using System;
using MongoDB.Bson;
using MongoDB.Bson.Serialization.Attributes;
using MongoDB.Driver;
using MongoDB.Driver.Builders;

namespace ScheduleService.Web.Models.Db
{
    public class MongoEntity    
    {
        public MongoEntity()
        {
            CreateDateTime = DateTime.Now;
            UpdateDateTime = DateTime.Now;
        }

        public BsonObjectId Id { get; set; }

        public IMongoQuery QueryById
        {
            get { return Query.EQ("_id", Id); }
        }

        [BsonDateTimeOptions(Kind = DateTimeKind.Local)]
        public DateTime UpdateDateTime { get; set; }

        [BsonDateTimeOptions(Kind = DateTimeKind.Local)]
        public DateTime CreateDateTime { get; set; }

    }
}