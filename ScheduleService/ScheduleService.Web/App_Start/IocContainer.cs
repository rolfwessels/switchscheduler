using System.Web.Mvc;
using Autofac;
using Autofac.Integration.Mvc;
using ScheduleService.Web.Controllers;
using ScheduleService.Web.Core;

namespace ScheduleService.Web.App_Start
{
    public static class IocContainer
    {
        private static IContainer _current;

        public static void Register()
        {
            var builder = new ContainerBuilder();
            RegisterControllers(builder);
            RegisterTools(builder);
            
            _current = builder.Build();
            DependencyResolver.SetResolver(new AutofacDependencyResolver(_current));   
        }

        private static void RegisterTools(ContainerBuilder builder)
        {
            builder.RegisterType<MongoDbContainer>().As<IMongoDbContainer>();
            builder.RegisterType<PlayTextFormatter>();
        }

        private static void RegisterControllers(ContainerBuilder builder)
        {
            builder.RegisterControllers(typeof(AutoMapperSetup).Assembly);
            builder.RegisterType<ScheduleController>();
        }

        public static IContainer Current
        {
            get { return _current; }
        }
    }
}