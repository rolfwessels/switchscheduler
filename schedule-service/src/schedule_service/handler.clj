(ns schedule-service.handler
  (:use [ring.adapter.jetty             :only [run-jetty]]
        [compojure.core                 :only [defroutes GET POST]]
        [ring.middleware.params         :only [wrap-params]]
        [hiccup.core]
        [hiccup.page]
        [schedule-service.middleware]
        [ring.middleware.file]
        [ring.middleware.file-info]
        [ring.middleware.stacktrace]
        )

  (:require 
  			[compojure.route :as route]      
  			[compojure.handler :as handler]
        [compojure.core :refer [defroutes GET POST]]
        ))


(defn view-layout [& content]
  (html
    (doctype :xhtml-strict)
    (xhtml-tag "en"
      [:head
        [:meta {:http-equiv "Content-type"
                :content "text/html; charset=utf-8"}]
        [:title "Schedule service"]]
        [:link {:href "/css/schedule_service.css" :rel "stylesheet" :type "text/css"}]
        [:link {:href "/css/bootstrap.min.css" :rel "stylesheet" :type "text/css"}]
      [:body 
        content
        [:script {:src "http://code.jquery.com/jquery-latest.js"} ]
        [:script {:src "js/bootstrap.min.js"} ]
      ]
      
      )))

(defn view-not-found []
  {
    :status 404
    :headers {}
    :body (view-layout
    [:div.container.fourOwFour

      [:h2  "Oopppss that page does not exist"]

      ])})

(defn view-input [& [a b]]
  (view-layout
    [:div.container
      
      [:form.form-signin {:method "POST" :action "/" }
        [:h2.form-signin-heading  "Update schedule"]
        [:input.input-block-level {:type "text" :name "mac" :placeholder "Device MAC address"}]
        [:br]
        [:button.btn.btn-large.btn-primary { :type "submit" } "Sign in"]
        ]]))

(defn view-output [mac]
  (view-layout
    [:div.container 
      
      [:form {:method "POST" :action "/" }
        [:h2.form-signin-heading  (str "Welcome "  mac)]
        [:input.input-block-level {:type "text" :name "mac" :placeholder "Device MAC address"}]
        [:br]
        [:button.btn.btn-large.btn-primary {:type "submit" } "Update"]
        ]]

  ))


(defroutes routes
  (GET "/" []
       (view-input))
  (POST "/ke" {params :params} 
    (str "POST id=" (params "id") " params=" params))
  (POST "/" [mac]
    (view-output mac))
  (route/not-found
   (view-not-found)))

(def production?
  (= "production" (get (System/getenv) "APP_ENV")))

(def development?
  (not production?))

(def app
  (-> #'routes
    (wrap-file "public")
    (wrap-params)
    (wrap-file-info)
    (wrap-request-logging)
    (wrap-bounce-favicon)
    (wrap-exception-logging)
;    (wrap-if production?  wrap-failsafe)
    (wrap-if development? wrap-stacktrace)
    ))

; (defn -main []
;   (run-jetty app {:port (if (nil? (System/getenv "PORT")) 
;                           8080 ; localhost or heroku?
;                           (Integer/parseInt (System/getenv "PORT")))}) )
(defn -main []
  (run-jetty app
                   {:port (Integer. (or (System/getenv "PORT") "8080"))
                    :join? false}))
