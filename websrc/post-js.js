//
// Doing web specific sanitization and gui "view controller" stuff
//

var output = document.getElementById("output");
var input = document.getElementById("input");

input.onkeydown = (aKey) =>
{
    if (aKey.key === "Enter")
    {
        output.value = "";

        var args = input.value.split(" "); 

        var MAX_LENGTH = 4;
            
        for (var i = args.length - 1; i--;)
        {
            if ( args[i] === "") args.splice(i, 1);
        }

        if (args.length > MAX_LENGTH) 
        {
            //args = args.splice(0, MAX_LENGTH); //silently trimming excess

            output.value = "Input sets greater than 4 are not supported on web build. The amount of solutions [can] reach into the hundreds, which produces an enormous output set. Calculating this is not an issue, but printing it in this output box can take minutes. If you want to calculate large input sets, build the native application and run it from an interactive shell";
        }
        else
        {
            /*args.forEach(function(i) 
            {
                if (i === "") args = args.splice
            });*/


            console.log(args);
            //Module.callMain(args);
        }
    }
};

