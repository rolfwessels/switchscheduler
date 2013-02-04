using System;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Net.Http.Formatting;
using System.Net.Http.Headers;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using ScheduleService.Web.Core;
using ScheduleService.Web.Models;
using ScheduleService.Web.Models.Db;
using log4net;

namespace ScheduleService.Web.App_Start
{
    public class PlayTextFormatter : MediaTypeFormatter
    {
        private static readonly ILog Log =
            LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);
        private static MongoContainer _mongoContainer = new MongoContainer("mongodb://appharbor_f9a8cacb-f25c-48f1-abee-6150ab31592d:g9fchakr68ijuhclojdst3uj76@ds037007.mongolab.com:37007/appharbor_f9a8cacb-f25c-48f1-abee-6150ab31592d");

        public PlayTextFormatter()
        {
            SupportedMediaTypes.Add(new MediaTypeHeaderValue("text/plain"));
        }

        public override bool CanReadType(Type type)
        {
            return type == typeof(string);
        }

        public override bool CanWriteType(Type type)
        {
            return type == typeof(ScheduleModel);
        }

        public override Task WriteToStreamAsync(Type type, object value, Stream writeStream, HttpContent content, TransportContext transportContext)
        {
            var task = Task.Factory.StartNew(() =>
            {
                var scheduleModel = value as ScheduleModel;
                var streamWriter = new StreamWriter(writeStream);
                if (scheduleModel != null)
                {
                    var tillUtc = DateTime.Parse(scheduleModel.RunTillUtc);
                    var runTillUtc = tillUtc < DateTime.Now.ToUniversalTime()? "0" : "1";
                    var datesToCode = new StringBuilder();
                    for (int day = 0; day < 7; day++)
                    {
                        for (int j = 0; j < 23; j++)
                        {
                            datesToCode.Append(scheduleModel.Days[day].Hours.Contains(j)?"1":"0");
                        }
                    }
                    streamWriter.Write("{" + runTillUtc + datesToCode.ToString() + "}");
                    Task.Factory.StartNew(() =>
                        {
                            //save the LastConnectTime
                            var scheduleDb = _mongoContainer.Schedule.Find(scheduleModel.QueryByMacAddress()).FirstOrDefault();
                            if (scheduleDb != null)
                            {
                                Log.Debug(string.Format("PlayTextFormatter:WriteToStreamAsync Update last connect time for {0}", scheduleDb.MacAddress));
                                scheduleDb.LastConnectTime = DateTime.Now;
                                _mongoContainer.Schedule.Save(scheduleDb);
                            }
                        });
                }
                
                streamWriter.Flush();
            });
            return task;

        }

        public override Task<object> ReadFromStreamAsync(Type type, Stream readStream, HttpContent content, IFormatterLogger formatterLogger)
        {
            var taskCompletionSource = new TaskCompletionSource<object>();
            try
            {
                var memoryStream = new MemoryStream();
                readStream.CopyTo(memoryStream);
                var s = System.Text.Encoding.UTF8.GetString(memoryStream.ToArray());
                taskCompletionSource.SetResult(s);
            }
            catch (Exception e)
            {
                taskCompletionSource.SetException(e);
            }
            return taskCompletionSource.Task;
        }

        
    }

   
    


}