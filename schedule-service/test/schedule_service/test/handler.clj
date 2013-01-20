(ns schedule-service.test.handler
  (:use clojure.test
        ring.mock.request  
        schedule-service.handler))

(deftest view-layout-valid
  (let [resp (routes {:uri "/" :request-method :get})]
    (is (= 200 (:status resp)))
    (is (re-find #"Schedule service" (:body resp)))
    (is (re-find #"title" (:body resp)))
    (is (re-find #"css" (:body resp)))))

(deftest view-input-valid
  (let [resp (routes {:uri "/" :request-method :get})]
    (is (= 200 (:status resp)))
    (is (re-find #"Update schedule" (:body resp)))
    (is (re-find #"<input" (:body resp)))
    (is (re-find #"mac" (:body resp)))))


(deftest view-output-valid
  (let [resp (routes {:uri "/" :request-method :post
                       :params {"mac" "0121212"}})]
    (is (= 200 (:status resp)))
    (is (re-find #"Welcome 0121212" (:body resp)))))

(deftest file-not-found-valid
  (testing "not-found routes"
    (let [resp (routes (request :get "/invalid"))]
      (is (= (:status resp) 404))
      (is (re-find #"not exist" (:body resp)))
    )))