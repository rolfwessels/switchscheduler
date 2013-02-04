var pageData = {};
$(document).ready(function() {
	
	$('#mac-input').submit(function() 
	{
		pageData.macaddress  = $('#mac').val();
		loadPage('static/schedule-setup.html',scheduleSetupLoaded);
		return false;
	});
	pageData.macaddress  = $('#mac').val();
	loadPage('static/schedule-setup.html?12', scheduleSetupLoaded);

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
    $('#get-button').click(function () {
        $.ajax({
            url: "/api/schedule/" + pageData.macaddress.replace(/:/g, ""),
            type: "GET",
            dataType: "text",
            success: function (data) {
                console.log(data);
                $(".container").append("result:[" + data + "]<br />");
            }
        });
    });
    

    $.ajax({
        url: "/api/schedule/" + pageData.macaddress.replace(/:/g, ""),
        type: "GET",
        contentType: "application/json; charset=utf-8",
        
        dataType: "json",
        success: function (data) {
            
            console.log(data);
            buildFormFromJson(data);
        }
    });

	$('#schedulesetup').submit(function() {
	    var postUrl = "/api/schedule/" + pageData.macaddress.replace(/:/g, "");
		var jsonPost = JSON.stringify(buildJsonFromForm()) ;
		$(".container").append("Url:"+postUrl+"<br />");
		$(".container").append("jsonPost:" + jsonPost + "<br />");

	    $.ajax({
	        url: postUrl,
	        type: "POST",
	        data: jsonPost,
	        contentType: "application/json; charset=utf-8",
	        dataType: "json",
	        success: function(data) {
	            alert(data);
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
	if ($('#on-now').attr('checked')) 
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
				if ($('#check_'+day+'_'+hour).attr('checked')) {
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