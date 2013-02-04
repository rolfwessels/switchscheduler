git update-index --assume-unchanged ScheduleService.Web\Web.config
git update-index --assume-unchanged ScheduleService.Web.Tests\app.config

copy D:\Dropbox\Work\Home\ScheduleService\app.config ScheduleService.Web.Tests\app.config
copy D:\Dropbox\Work\Home\ScheduleService\Web.config ScheduleService.Web\Web.config

rem git update-index --no-assume-unchanged ScheduleService.Web\Web.config
rem git update-index --no-assume-unchanged ScheduleService.Web.Tests\app.config
rem copy ScheduleService.Web\Web.config D:\Dropbox\Work\Home\ScheduleService
rem copy ScheduleService.Web.Tests\app.config D:\Dropbox\Work\Home\ScheduleService