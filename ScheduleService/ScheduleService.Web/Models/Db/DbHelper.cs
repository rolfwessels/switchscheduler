using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using MongoDB.Driver;
using MongoDB.Driver.Builders;

namespace ScheduleService.Web.Models.Db
{
    public static class ScheduleHelper
    {
        public static IMongoQuery QueryByMacAddress(this ISchedule schedule)   
        {
            return Query.EQ("MacAddress", schedule.MacAddress);
        }

        public static string ToMac(this string macString)
        {
            if (macString == null) throw new ArgumentNullException("macString");
            if (macString.Length < 12) throw new ArgumentOutOfRangeException("macString");
            if (macString.Length == 17) return macString;

            return macString.SplitByLength(2).StringJoin(":"); 
        }

        public static IEnumerable<string> SplitByLength(this string macString, int length)
        {
            for (int i = 0; i < macString.Length; i+= length)
            {
                yield return macString.Substring(i, length);
            }
        }

        public static string StringJoin<T>(this IEnumerable<T> strings, string separator)
        {
            if (strings == null) return null;
            return string.Join(separator, strings.Select(x => x.ToString()).ToArray());
        }

        public static DateTime ToLocalTime(this long runTillUtc)
        {

            return (new DateTime(1970, 01, 01).ToLocalTime().AddMilliseconds(runTillUtc));
        }

            
    }
}