using System;
using System.Diagnostics;
using FluentAssertions;
using NUnit.Framework;
using ScheduleService.Web.Models.Db;

namespace ScheduleService.Web.Tests.Core
{
    [TestFixture]
    public class ScheduleHelperTests
    {

        [Test]
        public void IdToMac_ValidId()
        {
            var idToMac = ScheduleHelper.ToMac("3DF2C9A6B34F");
            idToMac.Should().Be("3D:F2:C9:A6:B3:4F");
        }

        [Test]
        public void IdToMac_ValidMac()
        {
            var idToMac = ScheduleHelper.ToMac("3D:F2:C9:A6:B3:4F");
            idToMac.Should().Be("3D:F2:C9:A6:B3:4F");
        }

        [Test]
        public void IdToMac_TooShort()
        {
            Action call = () => ScheduleHelper.ToMac("3DF2C9A6B34");
            call.ShouldThrow<ArgumentOutOfRangeException>();
        }

        [Test]
        public void IdToMac_NullValue()
        {
            Action call = () => ScheduleHelper.ToMac(null);
            call.ShouldThrow<ArgumentNullException>();
        }
    }

}