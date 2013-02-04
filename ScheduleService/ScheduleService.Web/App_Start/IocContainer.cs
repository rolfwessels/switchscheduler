using System.Reflection;
using System.Web.Http;
using System.Web.Mvc;
using Autofac;
using Autofac.Integration.Mvc;
using Autofac.Integration.WebApi;
using ScheduleService.Web.Controllers;
using ScheduleService.Web.Core;
using log4net.Core;
using log4net.Repository.Hierarchy;

namespace ScheduleService.Web.App_Start
{
    public static class IocContainer
    {
        private static IContainer _current;

        public static void Register()
        {
            var builder = new ContainerBuilder();
            RegisterTools(builder);
            RegisterControllers(builder);
            _current = builder.Build();
            DependencyResolver.SetResolver(new AutofacDependencyResolver(_current));
            GlobalConfiguration.Configuration.DependencyResolver = new AutofacWebApiDependencyResolver(_current);
        }

        private static void RegisterTools(ContainerBuilder builder)
        {
            builder.RegisterType<MongoDbContainer>().As<IMongoDbContainer>();
            builder.RegisterType<PlayTextFormatter>();
            builder.RegisterType<ScheduleController>();
        }

        private static void RegisterControllers(ContainerBuilder builder)
        {
            builder.RegisterControllers(Assembly.GetExecutingAssembly());
            builder.RegisterApiControllers(Assembly.GetExecutingAssembly());
            builder.RegisterApiControllers(typeof(ScheduleController).Assembly);
            
        }

        public static IContainer Current
        {
            get { return _current; }
        }
    }
}