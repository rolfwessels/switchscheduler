using MongoDB.Driver;
using ScheduleService.Web.Models.Db;

namespace ScheduleService.Web.Core
{
    public interface IMongoDbContainer
    {
        MongoCollection<ScheduleDb> Schedule { get; }
    }
}