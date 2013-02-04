using System;
using System.Collections.Generic;
using MongoDB.Bson.Serialization.Attributes;
using MongoDB.Driver;
using MongoDB.Driver.Builders;

namespace ScheduleService.Web.Models.Db
{
    public class ScheduleDb : MongoEntity, ISchedule
    {
        public string MacAddress { get; set; }

        [BsonDateTimeOptions(Kind = DateTimeKind.Local)]
        public DateTime LastConnectTime { get; set; }

        [BsonDateTimeOptions(Kind = DateTimeKind.Local)]
        public DateTime LastNotifyTime { get; set; }

        [BsonDateTimeOptions(Kind = DateTimeKind.Local)]
        public DateTime RunTill { get; set; }

        public List<Day> Days { get; set; }

        public IMongoQuery QueryByMacAddress    
        {
            get { return Query.EQ("MacAddress", MacAddress); }
        }

        public override string ToString()
        {
            return string.Format("MacAddress: {0}, RunTill: {1} Schedule: {2}", MacAddress, RunTill , Days.StringJoin(":"));
        }

        #region Nested type: Day

        public class Day
        {
            public List<int> Hours { get; set; }

            #region Equals and ToString

            public override string ToString()
            {
                return string.Format("Hours: {0}", Hours.StringJoin("-"));
            }

            #endregion
        }

        #endregion
    }
}