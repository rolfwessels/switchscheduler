using System;
using System.Linq;
using System.Web.Optimization;
using AutoMapper;
using FluentAssertions;
using NUnit.Framework;
using ScheduleService.Web.App_Start;

namespace ScheduleService.Web.Tests.App_Start
{
    [TestFixture]
    public class BundleConfigTests 
    {

        #region Public Methods

        [Test]
        [Ignore]
        public void RegisterBundles()
        {
            BundleConfig.RegisterBundles(BundleTable.Bundles);
            BundleTable.Bundles.Count(x => x is ScriptBundle).Should().Be(2);
        }

        #endregion

        #region Properties

        #endregion
    }
}