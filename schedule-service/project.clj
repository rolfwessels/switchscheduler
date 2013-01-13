(defproject schedule-service "0.1.0-SNAPSHOT"
  :description "Clojure scheduling service"
  :url "https://github.com/rolfwessels/switchscheduler"
  :dependencies [[org.clojure/clojure "1.4.0"]
                 [compojure "1.1.5"]]
  :plugins [[lein-ring "0.8.0"]]
  :ring {:handler schedule-service.handler/app}
  :profiles
  {:dev {:dependencies [[ring-mock "0.1.3"]]}})
