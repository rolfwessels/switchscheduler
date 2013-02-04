using System;
using System.Globalization;
using System.Linq;
using FluentAssertions;
using MongoDB.Driver;
using MongoDB.Driver.Builders;
using MongoDB.Driver.Linq;
using NUnit.Framework;
using ScheduleService.Web.Core;
using ScheduleService.Web.Models.Db;

namespace ScheduleService.Web.Tests.Core
{
    [TestFixture]
    [Category("Integration")]
    public class MongoDbContainerTests
    {
        private readonly MongoDbContainer _mongoDbContainer;
        private const string MacName = "TestInsert";
        private DateTime _addHours = new DateTime(2001,02,02);
        private readonly IMongoQuery _mongoQuery;

        public MongoDbContainerTests()
        {
            _mongoDbContainer = new MongoDbContainer();
            _mongoQuery = Query.EQ("MacAddress", MacName);
            _mongoDbContainer.Schedule.Remove(_mongoQuery);
        }

        [Test]
        public void ConnectionString_Correct()
        {
            _mongoDbContainer.ConnectionString.Should().Contain("mongodb");
            
        }

        [Test]
        public void A_Insert()
        {
            long count = _mongoDbContainer.Schedule.Find(_mongoQuery).Count();
            _mongoDbContainer.Schedule.Find(_mongoQuery).Count().Should().Be(0);
            _mongoDbContainer.Schedule.Insert(new ScheduleDb() {MacAddress = MacName , RunTill = _addHours});
            _mongoDbContainer.Schedule.Find(_mongoQuery).Count().Should().Be(count + 1);
        }

        [Test]
        public void B_Find()
        {
            long count = _mongoDbContainer.Schedule.Find(_mongoQuery).Count();
            count.Should().BeGreaterOrEqualTo(1);
            _mongoDbContainer.Schedule.AsQueryable().Any(x => x.MacAddress == MacName)
                           .Should()
                           .BeTrue();
        }

        [Test]
        public void C_Update()
        {
            long count = _mongoDbContainer.Schedule.Find(_mongoQuery).Count();
            ScheduleDb found = _mongoDbContainer.Schedule.AsQueryable().FirstOrDefault(x => x.MacAddress == MacName);
            found.Should().NotBeNull();
            found.MacAddress.Should().Be(MacName);
            found.RunTill.ToString(CultureInfo.InvariantCulture).Should().Be(_addHours.ToString(CultureInfo.InvariantCulture));

            found.RunTill = _addHours.AddHours(1);
            _mongoDbContainer.Schedule.Save(found);

            ScheduleDb found2 = _mongoDbContainer.Schedule.AsQueryable().FirstOrDefault(x => x.MacAddress == MacName);
            found2.RunTill.ToString(CultureInfo.InvariantCulture).Should().Be(found.RunTill.ToString(CultureInfo.InvariantCulture));
        }

        [Test]
        public void D_Remove()
        {
            long count = _mongoDbContainer.Schedule.Find(_mongoQuery).Count();
            count.Should().BeGreaterOrEqualTo(1);
            ScheduleDb found = _mongoDbContainer.Schedule.AsQueryable().FirstOrDefault(x => x.MacAddress == MacName);
            found.Should().NotBeNull();
            _mongoDbContainer.Schedule.Remove(found.QueryById);
            _mongoDbContainer.Schedule.Find(_mongoQuery).Count().Should().Be(count - 1);
        }
    }

}