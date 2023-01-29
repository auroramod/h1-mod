main()
{
    replacefunc(maps\mp\gametypes\_class::isvalidprimary, ::isvalidprimary);
    replacefunc(maps\mp\gametypes\_class::isvalidsecondary, ::isvalidsecondary);
    replacefunc(maps\mp\gametypes\_class::isvalidweapon, ::isvalidweapon);
    replacefunc(maps\mp\gametypes\_class::buildweaponname, ::buildweaponname);
    replacefunc(maps\mp\gametypes\_weapons::watchweaponchange, ::watchweaponchange);
}

find_in_table(csv, weap)
{
    rows = tablegetrowcount(csv);

    for (i = 0; i < rows; i++)
    {
        if (tablelookupbyrow(csv, i, 0) == weap)
        {
            return true;
        }
    }

    return false;
}

get_attachment_override(weapon, attachment)
{
    csv = "mp/attachoverrides.csv";
    rows = tablegetrowcount(csv);

    if (!issubstr(weapon, "_mp"))
    {
        weapon += "_mp";
    }

    for (i = 0; i < rows; i++)
    {
        if (tablelookupbyrow(csv, i, 0) == weapon && tablelookupbyrow(csv, i, 1) == attachment)
        {
            return tablelookupbyrow(csv, i, 2);
        }
    }
}

get_attachment_name(weapon, attachment)
{
    name = tablelookup("mp/attachkits.csv", 1, attachment, 2);
    override = get_attachment_override(weapon, name);

    if (isdefined(override) && override != "")
    {
        return override;
    }

    return name;
}

is_custom_weapon(weap)
{
    return find_in_table("mp/customweapons.csv", weap);
}

watchweaponchange()
{
    self endon("death");
    self endon("disconnect");
    self endon("faux_spawn");
    thread maps\mp\gametypes\_weapons::watchstartweaponchange();
    self.lastdroppableweapon = self.currentweaponatspawn;
    self.hitsthismag = [];
    var_0 = self getcurrentweapon();

    if (maps\mp\_utility::iscacprimaryweapon(var_0) && !isdefined(self.hitsthismag[var_0]))
    {
        self.hitsthismag[var_0] = weaponclipsize(var_0);
    }

    self.bothbarrels = undefined;

    if (issubstr(var_0, "ranger"))
    {
        thread maps\mp\gametypes\_weapons::watchrangerusage(var_0);
    }

    var_1 = 1;

    for (;;)
    {
        if (!var_1)
        {
            self waittill("weapon_change");
        }

        var_1 = 0;
        var_0 = self getcurrentweapon();

        if (var_0 == "none")
        {
            continue;
        }

        var_2 = getweaponattachments(var_0);
        self.has_opticsthermal = 0;
        self.has_target_enhancer = 0;
        self.has_stock = 0;
        self.has_laser = 0;

        if (isdefined(var_2))
        {
            foreach (var_4 in var_2)
            {
                if (var_4 == "opticstargetenhancer")
                {
                    self.has_target_enhancer = 1;
                    continue;
                }

                if (var_4 == "stock")
                {
                    self.has_stock = 1;
                    continue;
                }

                if (var_4 == "lasersight")
                {
                    self.has_laser = 1;
                    continue;
                }

                if (issubstr(var_4, "opticsthermal"))
                {
                    self.has_opticsthermal = 1;
                }
            }
        }

        if (maps\mp\_utility::isbombsiteweapon(var_0))
        {
            continue;
        }

        var_6 = maps\mp\_utility::getweaponnametokens(var_0);
        self.bothbarrels = undefined;

        if (issubstr(var_0, "ranger"))
        {
            thread maps\mp\gametypes\_weapons::watchrangerusage(var_0);
        }

        if (var_6[0] == "alt")
        {
            var_7 = getsubstr(var_0, 4);
            var_0 = var_7;
            var_6 = maps\mp\_utility::getweaponnametokens(var_0);
        }

        if (var_0 != "none" && var_6[0] != "iw5" && var_6[0] != "iw6" && var_6[0] != "h1"  && var_6[0] != "h2")
        {
            if (maps\mp\_utility::iscacprimaryweapon(var_0) && !isdefined(self.hitsthismag[var_0 + "_mp"]))
            {
                self.hitsthismag[var_0 + "_mp"] = weaponclipsize(var_0 + "_mp");
            }
        }
        else if (var_0 != "none" && (var_6[0] == "iw5" || var_6[0] == "iw6" || var_6[0] == "h1" || var_6[0] == "h2"))
        {
            if (maps\mp\_utility::iscacprimaryweapon(var_0) && !isdefined(self.hitsthismag[var_0]))
            {
                self.hitsthismag[var_0] = weaponclipsize(var_0);
            }
        }

        if (maps\mp\gametypes\_weapons::maydropweapon(var_0))
        {
            self.lastdroppableweapon = var_0;
        }

        self.changingweapon = undefined;
    }
}

buildweaponname(var_0, var_1, var_2, var_3, var_4, var_5)
{
    if (!isdefined(var_0) || var_0 == "none" || var_0 == "")
    {
        return var_0;
    }

    if (!isdefined(level.lettertonumber))
    {
        level.lettertonumber = maps\mp\gametypes\_class::makeletterstonumbers();
    }

    var_6 = "";

    if (issubstr(var_0, "iw5_") || issubstr(var_0, "h1_") || issubstr(var_0, "h2_"))
    {
        var_7 = var_0 + "_mp";
        var_8 = var_0.size;

        if (issubstr(var_0, "h1_") || issubstr(var_0, "h2_"))
        {
            var_6 = getsubstr(var_0, 3, var_8);
        }
        else
        {
            var_6 = getsubstr(var_0, 4, var_8);
        }
    }
    else
    {
        var_7 = var_0;
        var_6 = var_0;
    }

    if (var_7 == "h1_junsho_mp")
    {
        var_1 = "akimbohidden";
    }

    var_9 = isdefined(var_1) && var_1 != "none";
    var_10 = isdefined(var_2) && var_2 != "none";

    if (!var_10)
    {
        var_11 = tablelookuprownum("mp/furniturekits/base.csv", 0, var_7);

        if (var_11 >= 0)
        {
            var_2 = "base";
            var_10 = 1;
        }
    }

    if (!issubstr(var_0, "h1_"))
    {
        if (var_9)
        {
            name = get_attachment_name(var_0, var_1);
            if (isdefined(name) && name != "")
            {
                var_7 += "_" + name;
            }
        }
    }
    else if (var_9 || var_10)
    {
        if (!var_9)
            var_1 = "none";

        if (!var_10)
            var_2 = "base";

        var_7 += ("_a#" + var_1);
        var_7 += ("_f#" + var_2);
    }

    if (issubstr(var_7, "iw5_") || issubstr(var_7, "h1_") || issubstr(var_7, "h2_"))
    {
        var_7 = maps\mp\gametypes\_class::buildweaponnamereticle(var_7, var_4);
        var_7 = maps\mp\gametypes\_class::buildweaponnameemblem(var_7, var_5);
        var_7 = maps\mp\gametypes\_class::buildweaponnamecamo(var_7, var_3);
        return var_7;
    }
    else if (!isvalidweapon(var_7 + "_mp"))
    {
        return var_0 + "_mp";
    }
    else
    {
        var_7 = maps\mp\gametypes\_class::buildweaponnamereticle(var_7, var_4);
        var_7 = maps\mp\gametypes\_class::buildweaponnameemblem(var_7, var_5);
        var_7 = maps\mp\gametypes\_class::buildweaponnamecamo(var_7, var_3);
        return var_7 + "_mp";
    }
}

isvalidweapon(var_0, var_1)
{
    if (!isdefined(level.weaponrefs))
    {
        level.weaponrefs = [];

        foreach (var_3 in level.weaponlist)
        {
            level.weaponrefs[var_3] = 1;
        }
    }

    if (isdefined(level.weaponrefs[var_0]))
    {
        return 1;
    }

    return 0;
}

isvalidsecondary(var_0, var_1, var_2)
{
    if (maps\mp\_utility::is_true(var_1))
    {
        return isvalidprimary(var_0);
    }

    if (maps\mp\_utility::islootweapon(var_0))
    {
        var_0 = maps\mp\gametypes\_class::getbasefromlootversion(var_0);
    }

    if (is_custom_weapon(var_0))
    {
        return true;
    }

    switch (var_0)
    {
        case "none":
        case "h1_beretta":
        case "h1_colt45":
        case "h1_deserteagle":
        case "h1_deserteagle55":
        case "h1_usp":
        case "h1_janpst":
        case "h1_aprpst":
        case "h1_augpst":
        case "h1_rpg":
            return 1;
        default:
            return 0;
    }

    return 0;
}


isvalidprimary(var_0, var_1)
{
    if (is_custom_weapon(var_0))
    {
        return true;
    }

    switch (var_0)
    {
        case "h1_ak47":
        case "h1_g3":
        case "h1_g36c":
        case "h1_m14":
        case "h1_m16":
        case "h1_m4":
        case "h1_mp44":
        case "h1_xmlar":
        case "h1_aprast":
        case "h1_augast":
        case "h1_ak74u":
        case "h1_mp5":
        case "h1_p90":
        case "h1_skorpion":
        case "h1_uzi":
        case "h1_febsmg":
        case "h1_aprsmg":
        case "h1_augsmg":
        case "h1_m1014":
        case "h1_winchester1200":
        case "h1_kam12":
        case "h1_junsho":
        case "h1_m60e4":
        case "h1_rpd":
        case "h1_saw":
        case "h1_feblmg":
        case "h1_junlmg":
        case "h1_barrett":
        case "h1_dragunov":
        case "h1_m21":
        case "h1_m40a3":
        case "h1_remington700":
        case "h1_febsnp":
        case "h1_junsnp":
            return 1;
        default:
            return 0;
    }

    return 0;
}
