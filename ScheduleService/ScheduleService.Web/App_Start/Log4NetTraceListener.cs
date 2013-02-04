using System.Diagnostics;
using log4net;
using log4net.Config;

namespace ScheduleService.Web.App_Start
{
    public class Log4NetTraceListener : TraceListener
    {
        private readonly ILog _log;

        public Log4NetTraceListener()
        {
            _log = LogManager.GetLogger("System.Diagnostics.Redirection");
        }

        public Log4NetTraceListener(ILog log)
        {
            _log = log;
        }

        public static void Register()
        {
            XmlConfigurator.Configure();
            Trace.Listeners.Add(new Log4NetTraceListener());
        }

        public override void Write(string message)
        {
            if (_log != null)
            {
                _log.Debug(message);
            }
        }

        public override void WriteLine(string message)
        {
            if (_log != null)
            {
                _log.Debug(message);
            }
        }
    }
}