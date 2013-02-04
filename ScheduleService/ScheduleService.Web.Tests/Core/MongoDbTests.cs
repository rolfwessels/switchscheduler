using System;
using System.Globalization;
using System.Linq;
using FluentAssertions;
using MongoDB.Driver.Builders;
using MongoDB.Driver.Linq;
using NUnit.Framework;
using ScheduleService.Web.Core;
using ScheduleService.Web.Models;
using ScheduleService.Web.Models.Db;

namespace ScheduleService.Web.Tests.Core
{
    //[TestFixture]
    public class MongoDbTests
    {
        private readonly MongoContainer _mongoContainer;
        private const string MacName = "TestInsert";
        private DateTime _addHours = new DateTime(2001,02,02);

        public MongoDbTests()
        {
            _mongoContainer = new MongoContainer();
            _mongoContainer.Schedule.Remove(Query.EQ("MacAddress", MacName));
        }

//        [Test]
        public void Connectionstring_Correct()
        {
            _mongoContainer.ConnectionString.Should().Contain("mongodb");
            
        }

//        [Test]
        public void A_Insert()
        {
            long count = _mongoContainer.Schedule.FindAll().Count();
            _mongoContainer.Schedule.FindAll().Count().Should().Be(0);
            _mongoContainer.Schedule.Insert(new ScheduleDb() {MacAddress = MacName , RunTill = _addHours});
            _mongoContainer.Schedule.FindAll().Count().Should().Be(count + 1);
        }

//        [Test]
        public void B_Find()
        {
            long count = _mongoContainer.Schedule.FindAll().Count();
            count.Should().BeGreaterOrEqualTo(1);
            _mongoContainer.Schedule.AsQueryable().Any(x => x.MacAddress == MacName)
                           .Should()
                           .BeTrue();
        }

//        [Test]
        public void C_Update()
        {
            long count = _mongoContainer.Schedule.FindAll().Count();
            ScheduleDb found = _mongoContainer.Schedule.AsQueryable().FirstOrDefault(x => x.MacAddress == MacName);
            found.Should().NotBeNull();
            found.MacAddress.Should().Be(MacName);
            found.RunTill.ToString(CultureInfo.InvariantCulture).Should().Be(_addHours.ToString(CultureInfo.InvariantCulture));

            found.RunTill = _addHours.AddHours(1);
            _mongoContainer.Schedule.Save(found);

            ScheduleDb found2 = _mongoContainer.Schedule.AsQueryable().FirstOrDefault(x => x.MacAddress == MacName);
            found2.RunTill.ToString(CultureInfo.InvariantCulture).Should().Be(found.RunTill.ToString(CultureInfo.InvariantCulture));
        }

//        [Test]
        public void D_Remove()
        {
            long count = _mongoContainer.Schedule.FindAll().Count();
            count.Should().BeGreaterOrEqualTo(1);
            ScheduleDb found = _mongoContainer.Schedule.AsQueryable().FirstOrDefault(x => x.MacAddress == MacName);
            found.Should().NotBeNull();
            _mongoContainer.Schedule.Remove(found.QueryById);
            _mongoContainer.Schedule.FindAll().Count().Should().Be(count - 1);


        }
    }

}