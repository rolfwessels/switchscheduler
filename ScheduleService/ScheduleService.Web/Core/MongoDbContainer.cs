using System.Configuration;
using MongoDB.Driver;
using ScheduleService.Web.Models;
using ScheduleService.Web.Models.Db;

namespace ScheduleService.Web.Core
{
    public class MongoDbContainer : IMongoDbContainer
    {
        private const string TableScheduleModel = "Schedule";
        private readonly string _connectionString;
        private readonly MongoUrl _url;
        private readonly MongoClient _client;
        private readonly MongoServer _server;
        private readonly MongoDatabase _database;
        private readonly MongoCollection<ScheduleDb> _schedule;

        public MongoDbContainer() : this(ConfigurationManager.AppSettings.Get("MONGOLAB_URI"))
        {
        
        }

        public MongoDbContainer(string connectionString)
        {
            _connectionString = connectionString;
            _url = new MongoUrl(connectionString);
            _client = new MongoClient(_url);
            _server = _client.GetServer();
            _database = _server.GetDatabase(_url.DatabaseName);

            _schedule = _database.GetCollection<ScheduleDb>(TableScheduleModel);
        }

        public MongoCollection<ScheduleDb> Schedule
        {
            get { return _schedule; }
        }

        public string ConnectionString
        {
            get { return _connectionString; }
        }

        public MongoUrl Url
        {
            get { return _url; }
        }

        public MongoClient Client
        {
            get { return _client; }
        }

        public MongoServer Server
        {
            get { return _server; }
        }

        public MongoDatabase Database
        {
            get { return _database; }
        }
    }
}