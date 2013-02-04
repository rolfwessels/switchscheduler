using System;
using System.Collections.Generic;
using MongoDB.Driver;
using MongoDB.Driver.Builders;
using ScheduleService.Web.Models.Db;

namespace ScheduleService.Web.Models
{
    public class ScheduleModel : ISchedule
    {
        public string MacAddress { get; set; }

        public string RunTillUtc { get; set; }

        public List<Day> Days { get; set; }

        public DateTime UpdateDateTime { get; set; }

        public DateTime CreateDateTime { get; set; }

        public DateTime LastConnectTime { get; set; }

        public DateTime LastNotifyTime { get; set; }

        #region Equals and ToString

        public override string ToString()
        {
            return string.Format("MacAddress: {0}, RunTill: {1}", MacAddress, RunTillUtc);
        }

        #endregion
        
        #region Nested type: Day

        public class Day
        {
            public string DayName { get; set; }
            public List<int> Hours { get; set; }
        }

        #endregion
    }
}