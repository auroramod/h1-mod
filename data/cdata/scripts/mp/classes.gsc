main()
{
    replacefunc(maps\mp\gametypes\_menus::getclasschoice, ::getclasschoice);
}

getclasschoice(choice)
{
    if (choice <= 100)
    {
        if (getdvar("sv_disableCustomClasses") == "1") 
        {
            return "class0";
        }

        choice = "custom" + choice;
    }
    else if (choice <= 200)
    {
        choice -= 101;
        choice = "class" + choice;
    }
    else if ( choice <= 206 )
    {
        choice -= 200;
        choice = "axis_recipe" + choice;
    }
    else
    {
        choice -= 206;
        choice = "allies_recipe" + choice;
    }

    return choice;
}
