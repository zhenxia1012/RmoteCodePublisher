function expand(expd, div)
{
    var elem = document.getElementById(expd);
    if (elem.innerHTML == "-")
    {
        //document.write("-----------");
        elem.innerHTML = "+";
        elem = document.getElementById(div);
        elem.style.display = "none";
    }
    else
    {
        //document.write("+++++++");
        elem.innerHTML = "-";
        elem = document.getElementById(div);
        elem.style.display = "inline";
    }
}