git update-index --assume-unchanged ScheduleService.Web\Web.config
git update-index --assume-unchanged ScheduleService.Web.Tests\app.config


rem copy D:\Dropbox\Work\Home\ScheduleService\app.config ScheduleService.Web.Tests\app.config
rem copy D:\Dropbox\Work\Home\ScheduleService\Web.config ScheduleService.Web\Web.config

git update-index --no-assume-unchanged ScheduleService.Web\Web.config
git update-index --no-assume-unchanged ScheduleService.Web.Tests\app.config

rem ScheduleService.Web\Web.config D:\Dropbox\Work\Home\ScheduleService
rem ScheduleService.Web.Tests\app.config D:\Dropbox\Work\Home\ScheduleService
