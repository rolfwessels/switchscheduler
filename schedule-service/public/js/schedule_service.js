var pageData = {};
$(document).ready(function() {
	
	$('#mac-input').submit(function() 
	{
		pageData.macaddress  = $('#mac').val();
		loadPage('html/schedule-setup.html',scheduleSetupLoaded);
		return false;
	});
	pageData.macaddress  = $('#mac').val();
	loadPage('html/schedule-setup.html',scheduleSetupLoaded);

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
					replaceTemplate(pageUrl)
				});
		}
		else {
			replaceTemplate(pageUrl);
		}
	} );	
}

function scheduleSetupLoaded() {
	$('#toggle-button').toggleButtons({animated: false});	
	$('#schedulesetup').submit(function() {
		var postUrl = "api/schedule/"+pageData.macaddress;
		postUrl = "api/schedule/";
		var jsonPost = JSON.stringify(buildJsonFromForm()) ;
		alert(jsonPost);
		$(".content").append();

		$.ajax({
		  url:postUrl,
		  type:"POST",
		  data:jsonPost,
		  contentType:"application/json; charset=utf-8",
		  dataType:"json",
		  success: function(data){
		    alert(data);
		  }
		})

		return false;
	});
}

function buildJsonFromForm() {
	var result = {};
	if ($('#on-now').attr('checked')) 
	{
		var date = new Date();
		date.setMinutes(date.getMinutes()+60);
		result.RunTill = date.getTime();
	}
	var days = ["Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"];
	try	
	{
		result.days = [];
		for (var day = 0; day < 7; day++) {
			var hours = [];
			for (var hour = 0; hour < 24; hour++) {
				if ($('#check_'+day+'_'+hour).attr('checked')) {
					hours.push(hour);
				}
			};
			var dayName = days[day];
			result.days[day] = {Day : dayName, Hours : hours } ;
		};
	}
	catch (e) {

	}
	return result;
}
