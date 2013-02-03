# schedule-service

A Clojure library designed to create a schedule file based on schedule input

To see it in action check out http://schedule-service.herokuapp.com/

## Prerequisites

You will need [Leiningen][1] 1.7.0 or above installed.

[1]: https://github.com/technomancy/leiningen

For upload to heroku https://toolbelt.heroku.com/

## Running

To start a web server for the application, run:

    lein ring server

## License

Copyright © 2013 Rolf Wessels

Distributed under the Eclipse Public License, the same as Clojure.


Setup heroku
================
in schedule-service folder
```
git init
git add .
git commit -a -m "Initial commit"

install https://toolbelt.heroku.com/

heroku login
cd C:\Users\yourusername\.ssh
ssh-keygen -t rsa -f id_rsa
heroku keys:add "id_rsa.pub"
heroku create schedule-service
git push heroku master

heroku logs -n 200
```