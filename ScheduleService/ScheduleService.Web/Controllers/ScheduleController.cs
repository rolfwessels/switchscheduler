using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Web.Http;
using AutoMapper;
using Autofac;
using MongoDB.Driver.Linq;
using ScheduleService.Web.App_Start;
using ScheduleService.Web.Core;
using ScheduleService.Web.Models;
using ScheduleService.Web.Models.Db;
using log4net;

namespace ScheduleService.Web.Controllers
{
    public class ScheduleController : ApiController
    {
        private readonly IMongoDbContainer _mongoDbContainer;
        private static readonly ILog _log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

//        public ScheduleController()
//            : this(IocContainer.Current.Resolve<IMongoDbContainer>())
//        {
//        }

        public ScheduleController(IMongoDbContainer mongoDbContainer) 
        {
             _mongoDbContainer = mongoDbContainer;
        }

        // GET api/schedule
        public IEnumerable<ScheduleModel> Get()
        {
            _log.Info(string.Format("ScheduleController:Get all"));
            return Mapper.Map<IEnumerable<ScheduleDb>, IEnumerable<ScheduleModel>>(Db.Schedule.FindAll());
        }
        
        // GET api/schedule/5
        public ScheduleModel Get(string id)
        {
            _log.Info(string.Format("ScheduleController:Get [{0}]", id.ToMac()));
            var scheduleDb = Db.Schedule.AsQueryable().FirstOrDefault(x => x.MacAddress == id.ToMac());
            return Mapper.Map<ScheduleDb,ScheduleModel>(scheduleDb); ;
        }

        // POST api/schedule
        public void Post([FromBody]ScheduleModel value)
        {
            _log.Info(string.Format("ScheduleController:UpdateOrInsert [{0}]", value));
            UpdateOrInsert(value);
        }

        // PUT api/schedule/mac
        public void Put(string id, [FromBody]ScheduleModel value)
        {
            _log.Info(string.Format("ScheduleController:UpdateOrInsert [{0}]", value));
            UpdateOrInsert(value);
        }

        // DELETE api/schedule/5
        public void Delete(string id)
        {
            if (id == null) throw new ArgumentNullException("id");
            Db.Schedule.Remove((new ScheduleDb() { MacAddress = id}).QueryByMacAddress);
        }

        #region Private Methods

        private void UpdateOrInsert(ScheduleModel value)
        {
            if (value == null) throw new ArgumentNullException("value");
            if (string.IsNullOrEmpty(value.MacAddress))
                throw new ArgumentNullException("value", "MacAddress cannot be null or empty");

            _log.Info(string.Format("ScheduleController:UpdateOrInsert [{0}]", value));
            var scheduleModels = Db.Schedule.Find(value.QueryByMacAddress()).FirstOrDefault();
            if (scheduleModels == null)
            {
                var insert = Mapper.Map<ScheduleModel, ScheduleDb>(value);
                LogTheSave(insert);
                Db.Schedule.Insert(insert);
            }
            else
            {
                Mapper.Map(value,scheduleModels);
                scheduleModels.UpdateDateTime = DateTime.Now;
                LogTheSave(scheduleModels);
                Db.Schedule.Save(scheduleModels);
            }
        }

        private void LogTheSave(ScheduleDb insert)
        {
            _log.Debug("ScheduleController:Saved " + insert);
        }

        protected IMongoDbContainer Db
        {
            get { return _mongoDbContainer; }
            
        }

        #endregion
    }
}