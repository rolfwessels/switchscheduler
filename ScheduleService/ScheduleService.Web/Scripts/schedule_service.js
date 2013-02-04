var pageData = {};
$(document).ready(function() {
	
	$('#mac-input').submit(function() 
	{
		pageData.macaddress  = $('#mac').val();
		loadPage('static/schedule-setup.html?2',scheduleSetupLoaded);
		return false;
	});
//	pageData.macaddress  = $('#mac').val();
//	loadPage('static/schedule-setup.html?12', scheduleSetupLoaded);

});

// adding for loop
(function ($) {
	$.extend(jQuery.tmpl.tag, {
	    "for": {
	        _default: {$2: "var i=1;i<=1;i++"},
	        open: 'for ($2){',
	        close: '};'
	    }
	});
	$.extend($.tmpl.tag, {
	    "var": {
	        open: "var $1;"
	    }
	});
})(jQuery);

$.fn.serializeObject = function()
{
    var o = {};
    var a = this.serializeArray();
    $.each(a, function() {
        if (o[this.name] !== undefined) {
            if (!o[this.name].push) {
                o[this.name] = [o[this.name]];
            }
            o[this.name].push(this.value || '');
        } else {
            o[this.name] = this.value || '';
        }
    });
    return o;
};

function loadPage(pageUrl,callBack) {
	$('.container').hide('slow',function() {
		var found = jQuery.template[pageUrl];
		
		function replaceTemplate(tmplName) {
			var x = $.tmpl(tmplName, pageData);
			$(".container").html(x);
			$('.container').show('slow');
			if (callBack != null) callBack();
		}

		if (found == null) {
				$.get(pageUrl, function(data) {
					$.template(pageUrl,data);
				    replaceTemplate(pageUrl);
				});
		}
		else {
			replaceTemplate(pageUrl);
		}
	} );	
}

function scheduleSetupLoaded() {
    $('.switch')['switch']();
    var url = "/api/schedule/" + pageData.macaddress.replace(/:/g, "");
    $('#btn-all').click(function () {
        for (var day = 0; day < 7; day++) {
            for (var hour = 0; hour < 24; hour++) {
                $('#check_' + day + '_' + hour).prop('checked', true);
            };
        };
    });

    $('#btn-none').click(function () {
        for (var day = 0; day < 7; day++) {
            for (var hour = 0; hour < 24; hour++) {
                $('#check_' + day + '_' + hour).prop('checked', false);
            };
        };
    });

    $('#btn-morning').click(function () {
        for (var day = 0; day < 7; day++) {
            for (var hour = 0; hour < 24; hour++) {
                $('#check_' + day + '_' + hour).prop('checked', hour == 6);
            };
        };
    });

    $('#btn-evening').click(function () {
        for (var day = 0; day < 7; day++) {
            for (var hour = 0; hour < 24; hour++) {
                $('#check_' + day + '_' + hour).prop('checked', hour == 18);
            };
        };
    });
   
    $('.dataTable').hide();
    console.log("Load schedule setup data");
    console.log("GET: "+ url);
    $.ajax({
        url: url,
        type: "GET",
        contentType: "application/json; charset=utf-8",
        dataType: "json",
        success: function (data) {
            console.log("GET Data:");
            console.log(data);
            buildFormFromJson(data);
            $('.dataTable').show('slow');
        }
    });

	$('#schedulesetup').submit(function() {
	    var postUrl = "/api/schedule/" + pageData.macaddress.replace(/:/g, "");
	    var jsonPost = JSON.stringify(buildJsonFromForm());

	    console.log("Url:");
	    console.log(postUrl);
	    console.log("jsonPost:");
	    console.log(jsonPost);
		
	    $.ajax({
	        url: postUrl,
	        type: "POST",
	        data: jsonPost,
	        contentType: "application/json; charset=utf-8",
	        dataType: "json",
	        success: function (data) {
	            console.log("POST Data:");
	            console.log(data);
	        }
	    });
	    return false;
	});
}

function buildFormFromJson(json) {
    pageData.MacAddress = json.MacAddress;
    
    var date = getLocalTimeFromUtc(json.RunTillUtc);
    var currentDate = new Date();
    var isRunHour = (json.RunTillUtc != 0) && (date.getTime() > currentDate.getTime());
    console.log(date);
    console.log(json.RunTillUtc);
    console.log(isRunHour);
    $('#on-now').prop('checked', isRunHour);
    $('#toggle-on-now').switch('setState', isRunHour);
    $('#lastConnected').html(ReableTime(json.LastConnectTime));
    
    try {
        for (var day = 0; day < 7; day++) {
            var hours = json.Days[day].Hours;
            console.log(hours);
            for (var hour = 0; hour < 24; hour++) {
                var isChecked = jQuery.inArray(hour, hours) != -1;
                $('#check_' + day + '_' + hour).prop('checked', isChecked);
            };
        };
    }
    catch (e) {

    }
}

function buildJsonFromForm() {
    var result = {};
    result.MacAddress = pageData.macaddress;
    if ($('#on-now').prop('checked'))
	{
	    var date = new Date();
		date.setMinutes(date.getMinutes()+60);
		result.RunTillUtc = date.toUTCString();
	}
	var days = ["Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"];
	try	
	{
		result.Days = [];
		for (var day = 0; day < 7; day++) {
			var hours = [];
			for (var hour = 0; hour < 24; hour++) {
			    if ($('#check_' + day + '_' + hour).prop('checked')) {
					hours.push(hour);
				}
			};
			var dayName = days[day];
			result.Days[day] = {DayName : dayName, Hours : hours } ;
		};
	}
	catch (e) {

	}
	return result;
}

function getUtcTime() {
    var now = new Date();
    var nowUtc = new Date(now.getUTCFullYear(), now.getUTCMonth(), now.getUTCDate(), now.getUTCHours(), now.getUTCMinutes(), now.getUTCSeconds());
    return nowUtc;
}

function getLocalTimeFromUtc(dateTics) {
    var offset = new Date().getTimezoneOffset();
    var date = new Date(dateTics);
    date.setMinutes(date.getMinutes() - offset);
    return date;
}

function ReableTime(inTime) {
    // TIP: to find current time in milliseconds, use:
    // var milliseconds_now = new Date().getTime();
    var milliseconds = new Date().getTime() - new Date(inTime).getTime();
    var seconds = milliseconds / 1000;
    var numyears = Math.floor(seconds / 31536000);
    var postFix = " ago";
    if (numyears > 10) {
        return "not yet connected";
    }
    if (numyears) {
        return numyears + ' year' + ((numyears > 1) ? 's' : '') + postFix;
    }
    var numdays = Math.floor((seconds % 31536000) / 86400);
    if (numdays) {
        return numdays + ' day' + ((numdays > 1) ? 's' : '') + postFix;
    }
    var numhours = Math.floor(((seconds % 31536000) % 86400) / 3600);
    if (numhours) {
        return numhours + ' hour' + ((numhours > 1) ? 's' : '') + postFix;
    }
    var numminutes = Math.floor((((seconds % 31536000) % 86400) % 3600) / 60);
    if (numminutes) {
        return numminutes + ' minute' + ((numminutes > 1) ? 's' : '') + postFix;
    }
    var numseconds = (((seconds % 31536000) % 86400) % 3600) % 60;
    if (numseconds) {
        return numseconds + ' second' + ((numseconds > 1) ? 's' : '') + postFix;
    }
    return 'less then a second'; //'just now' //or other string you like;
}