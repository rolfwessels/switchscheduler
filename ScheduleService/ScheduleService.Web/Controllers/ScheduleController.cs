using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Web.Http;
using AutoMapper;
using MongoDB.Driver.Linq;
using ScheduleService.Web.Core;
using ScheduleService.Web.Models;
using ScheduleService.Web.Models.Db;
using log4net;

namespace ScheduleService.Web.Controllers
{
    public class ScheduleController : ApiController
    {
        private static readonly ILog Log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);
        private MongoContainer _mongoContainer;

        // GET api/schedule
        public IEnumerable<ScheduleModel> Get()
        {
            Log.Info(string.Format("ScheduleController:Get all"));
            return Mapper.Map<IEnumerable<ScheduleDb>, IEnumerable<ScheduleModel>>(Db.Schedule.FindAll());
        }
        
        // GET api/schedule/5
        public ScheduleModel Get(string id)
        {
            Log.Info(string.Format("ScheduleController:Get [{0}]", id.ToMac()));
            var scheduleDb = Db.Schedule.AsQueryable().FirstOrDefault(x => x.MacAddress == id.ToMac());
            return Mapper.Map<ScheduleDb,ScheduleModel>(scheduleDb); ;
        }

        public ScheduleModel Update(string id)
        {
            Log.Info(string.Format("ScheduleController:Update [{0}]", id.ToMac()));
            var scheduleDb = Db.Schedule.AsQueryable().FirstOrDefault(x => x.MacAddress == id.ToMac());
            return Mapper.Map<ScheduleDb, ScheduleModel>(scheduleDb); ;
        }

        // POST api/schedule
        public void Post([FromBody]ScheduleModel value)
        {
            Log.Info(string.Format("ScheduleController:UpdateOrInsert [{0}]", value));
            UpdateOrInsert(value);
        }

        // PUT api/schedule/mac
        public void Put(string id, [FromBody]ScheduleModel value)
        {
            Log.Info(string.Format("ScheduleController:UpdateOrInsert [{0}]", value));
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

            Log.Info(string.Format("ScheduleController:UpdateOrInsert [{0}]", value));
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
            Log.Debug("ScheduleController:Saved " + insert);
        }

        protected MongoContainer Db
        {
            get { return _mongoContainer ?? (_mongoContainer = new MongoContainer("mongodb://appharbor_f9a8cacb-f25c-48f1-abee-6150ab31592d:g9fchakr68ijuhclojdst3uj76@ds037007.mongolab.com:37007/appharbor_f9a8cacb-f25c-48f1-abee-6150ab31592d")); }
            
        }

        #endregion
    }
}