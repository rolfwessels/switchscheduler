using System;
using System.Globalization;
using AutoMapper;
using ScheduleService.Web.Controllers;
using ScheduleService.Web.Models;
using ScheduleService.Web.Models.Db;

namespace ScheduleService.Web.App_Start
{
    public class AutoMapperSetup
    {
        public static void Register()
        {
            Mapper.CreateMap<ScheduleDb, ScheduleModel>()
                  .ForMember(x => x.RunTillUtc, opt => opt.MapFrom(x => x.RunTill.ToUniversalTime()));
            Mapper.CreateMap<ScheduleModel, ScheduleDb>()
                  .ForMember(x => x.Id, opt => opt.Ignore())
                  .ForMember(x => x.CreateDateTime, opt => opt.Ignore())
                  .ForMember(x => x.UpdateDateTime, opt => opt.Ignore())
                  .ForMember(x => x.RunTill,
                             opt =>
                             opt.MapFrom(
                                 x =>
                                 string.IsNullOrEmpty(x.RunTillUtc)
                                     ? new DateTime(1970)
                                     : DateTime.Parse(x.RunTillUtc, CultureInfo.InvariantCulture,
                                                      DateTimeStyles.AssumeUniversal)));
            Mapper.CreateMap<ScheduleDb.Day, ScheduleModel.Day>()
                  .ForMember(x => x.DayName, opt => opt.Ignore());
            Mapper.CreateMap<ScheduleModel.Day, ScheduleDb.Day>();
        }
    }
}