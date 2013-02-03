(defproject schedule-service "1.0.1"
  :description "Clojure scheduling service"
  :url "https://github.com/rolfwessels/switchscheduler"
  :dependencies [[org.clojure/clojure "1.4.0"]
                 [org.clojure/clojure-contrib "1.2.0"]
                 [compojure "1.1.1"]
                 [ring/ring-core "1.1.7"]
                 [ring/ring-devel "1.1.7"]
                 [ring/ring-jetty-adapter "1.1.7"]
                 [hiccup "1.0.2"]]
  :main ^:skip-aot schedule-service.handler
  :plugins [[lein-ring "0.8.0"]]
  :ring {:handler schedule-service.handler/app}
  :profiles {:dev {:dependencies [
    [ring-mock "0.1.3"]
    [lein-reload "1.0.0"]
    ]}}
  )
