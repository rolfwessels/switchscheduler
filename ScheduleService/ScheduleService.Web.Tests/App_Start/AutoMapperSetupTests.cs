using AutoMapper;
using FluentAssertions;
using NUnit.Framework;
using ScheduleService.Web.App_Start;
using ScheduleService.Web.Models;
using ScheduleService.Web.Models.Db;

namespace ScheduleService.Web.Tests.App_Start
{
    [TestFixture]
    public class AutoMapperSetupTests 
    {

        public AutoMapperSetupTests()
        {
            AutoMapperSetup.Register();
        }

 
        [Test]
        public void AssertConfigurationIsValid()
        {
            Mapper.AssertConfigurationIsValid();
        }

        [Test]
        [Ignore]
        public void ScheduleModelScheduleDb()
        {
            const string runTill = "Sun, 03 Feb 2013 23:05:31 GMT";
            var scheduleDb = Mapper.Map<ScheduleModel, ScheduleDb>(new ScheduleModel() {RunTillUtc = runTill});

            var dateTime = scheduleDb.RunTill.ToUniversalTime();
            dateTime.ToLongTimeString().Should().Be("23:05:31");
            dateTime.ToLongDateString().Should().Be("03 February 2013");
            
            var scheduleModel = Mapper.Map<ScheduleDb, ScheduleModel>(scheduleDb);
            scheduleModel.RunTillUtc.Should().Contain("23:05:31");
            scheduleModel.RunTillUtc.Should().Contain("03/02/2013");

        }
    }
}