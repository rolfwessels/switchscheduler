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
        [clojure.java.io]
        

        )
 
  (:require 
  			[compojure.route :as route]      
  			[compojure.handler :as handler]
        [compojure.core :refer [defroutes GET POST]]
        ))

; [clojure.contrib.duck-streams]

(defn view-layout [& content]
  (html5
    (doctype :xhtml-strict)
    (xhtml-tag "en"
      [:head
        [:meta {:http-equiv "Content-type"
                :content "text/html; charset=utf-8"}]
        [:title "Schedule service"]]
        [:link {:href "/css/schedule_service.css" :rel "stylesheet" :type "text/css"}]
        [:link {:href "/css/bootstrap.min.css" :rel "stylesheet" :type "text/css"}]
        [:link {:href "/css/bootstrap-toggle-buttons.css" :rel "stylesheet" :type "text/css"}]
      [:body 
        content
        [:script {:src "http://code.jquery.com/jquery-latest.js"} ]
        [:script {:src "js/bootstrap.min.js"} ]
        [:script {:src "js/jquery.toggle.buttons.js"} ]
        [:script {:src "js/jquery.tmpl.min.js"} ]
        [:script {:src "js/jquery.tmplPlus.min.js"} ]
        ;[:script {:src "js/jquery.tools.min.js"} ]
        ;[:script {:src "js/forms.js"} ]
        [:script {:src "js/schedule_service.js"} ]
      ]      
      )))

(defn view-not-found []
  {
    :status 404
    :headers {}
    :body (view-layout
    [:div.container.fourOwFour

      [:h2  "Oooopsss that page does not exist"]
      [:p "Ke? "]

      ])})

(defn view-input []
  (view-layout
    [:div.container
      
      [:form.form-signin {:method "POST" :action "/" :id "mac-input"  }
        [:h2.form-signin-heading  "Update schedule"]
        [:input.input-block-level {:type "text" :name "mac" :id "mac" 
          :required "required" 
          :data-message "Please enter valid mac address in format eg.3D:F2:C9:A6:B3:4F "
          :placeholder "Device MAC address" 
          :pattern "^([0-9A-F]{2}[:-]){5}([0-9A-F]{2})$"
          :value "3D:F2:C9:A6:B3:4F"
          }]
        [:br]
        [:button.btn.btn-large.btn-primary { :type "submit" } "Sign in"]
        ]]))

(defn save-schedule [mac content]
  (let [filename   (str "assets/schedules/" mac ".json")  ]
        (prn filename)
        (with-open [wrtr (writer filename)]
          (.write wrtr content))
    )
  (str "mac [" mac "] Content:" content)
  )

(defroutes routes
  (GET "/" []
       (view-input))
  (POST "/" []
       (view-input))
  (POST "/ke" {params :params} 
    (str "POST id=" (params "id") " params=" params))
  (POST "/api/schedule1/:mac" [& params]
    (str "params:" params))
  (POST "/api/schedule/" {request :body}
    (save-schedule "" (str request)))
  ; (POST "/api/schedule/" request
  ;   (str  ((json/parse-string 
  ;         (slurp* (:body request))) "hash-key"))))

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
