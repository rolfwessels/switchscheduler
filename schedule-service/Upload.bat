call lein test
git commit -a -m "Upload"
git push heroku master
rem sleep
ping 1.1.1.1 -n 1 -w 4000 > nul
heroku logs --tail