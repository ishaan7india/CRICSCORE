#include "ScoringEngine.h"

bool ScoringEngine::validate(JsonObjectConst e, String &error) {
  const char *kind=e["kind"]|""; const int runs=e["runs"]|0;
  const bool known=!strcmp(kind,"runs")||!strcmp(kind,"wicket")||!strcmp(kind,"wide")||!strcmp(kind,"no_ball")||!strcmp(kind,"bye")||!strcmp(kind,"leg_bye");
  if(!known){error="Unknown event kind";return false;} if(runs<0||runs>6){error="Runs must be 0..6";return false;}
  if(!strcmp(kind,"runs")&&runs==5){error="Batter runs must be 0,1,2,3,4,6";return false;}
  if(e.containsKey("wicket")&&(!strcmp(kind,"runs")||!strcmp(kind,"bye")||!strcmp(kind,"leg_bye"))){error="Dismissal is not valid for this event kind";return false;}
  if(!strcmp(kind,"wicket")&&runs>0&&e["wicket"].as<String>()!="run_out"&&e["wicket"].as<String>()!="obstructing_field"){error="Completed runs are only valid with run-outs";return false;}
  if((!strcmp(kind,"wicket")||e.containsKey("wicket"))&&!e["wicket"].is<const char*>()){error="Dismissal type required";return false;}
  if(e.containsKey("wicket")){const char*w=e["wicket"];if(strcmp(w,"bowled")&&strcmp(w,"caught")&&strcmp(w,"lbw")&&strcmp(w,"run_out")&&strcmp(w,"stumped")&&strcmp(w,"hit_wicket")&&strcmp(w,"obstructing_field")){error="Unsupported dismissal type";return false;}}
  if(!strcmp(kind,"wide")&&e.containsKey("wicket")&&strcmp(e["wicket"],"run_out")&&strcmp(e["wicket"],"stumped")){error="Illegal wide dismissal";return false;}
  if(!strcmp(kind,"no_ball")&&e.containsKey("wicket")&&strcmp(e["wicket"],"run_out")&&strcmp(e["wicket"],"obstructing_field")){error="Illegal no-ball dismissal";return false;}
  if(!e["batterId"].is<const char*>()||!e["bowlerId"].is<const char*>()){error="Batter and bowler are required";return false;} return true;
}

void ScoringEngine::apply(JsonObjectConst e, JsonObject s) {
  String kind=e["kind"].as<String>(); int runs=e["runs"]|0; int total=s["total"]|0;int legalBefore=s["legalBalls"]|0;
  if(!s["overs"].is<JsonArray>())s["overs"].to<JsonArray>();if(!s["fow"].is<JsonArray>())s["fow"].to<JsonArray>();if(!s["progression"].is<JsonArray>())s["progression"].to<JsonArray>();if(!s["partnerships"].is<JsonArray>())s["partnerships"].to<JsonArray>();if(!s["currentPartnership"].is<JsonObject>()){JsonObject p=s["currentPartnership"].to<JsonObject>();p["batters"][0]=s["striker"];p["batters"][1]=s["nonStriker"];p["runs"]=0;p["balls"]=0;}
  int partnershipStart=total;
  JsonObject ex=s["extras"].as<JsonObject>(); JsonObject bat=s["batters"][e["batterId"].as<const char*>()].as<JsonObject>();
  JsonObject bowl=s["bowlers"][e["bowlerId"].as<const char*>()].as<JsonObject>();
  bool legal=kind!="wide"&&kind!="no_ball";
  if(kind=="wide"){total+=1+runs;ex["wides"]=(int)ex["wides"]+1+runs;bowl["conceded"]=(int)bowl["conceded"]+1+runs;}
  else if(kind=="no_ball"){total+=1+runs;ex["noBalls"]=(int)ex["noBalls"]+1;bat["runs"]=(int)bat["runs"]+runs;if(runs==4)bat["fours"]=(int)bat["fours"]+1;if(runs==6)bat["sixes"]=(int)bat["sixes"]+1;bowl["conceded"]=(int)bowl["conceded"]+1+runs;}
  else if(kind=="bye"||kind=="leg_bye"){total+=runs;ex[kind=="bye"?"byes":"legByes"]=(int)ex[kind=="bye"?"byes":"legByes"]+runs;bat["balls"]=(int)bat["balls"]+1;bowl["balls"]=(int)bowl["balls"]+1;if(runs==0)bowl["dots"]=(int)bowl["dots"]+1;}
  else {int scored=runs;total+=scored;bat["runs"]=(int)bat["runs"]+scored;bat["balls"]=(int)bat["balls"]+1;bowl["balls"]=(int)bowl["balls"]+1;bowl["conceded"]=(int)bowl["conceded"]+scored;if(scored==0)bowl["dots"]=(int)bowl["dots"]+1;if(scored==4)bat["fours"]=(int)bat["fours"]+1;if(scored==6)bat["sixes"]=(int)bat["sixes"]+1;}
  s["total"]=total;if(legal)s["legalBalls"]=(int)s["legalBalls"]+1;
  if(e.containsKey("wicket")){s["wickets"]=(int)s["wickets"]+1;String w=e["wicket"].as<String>();if(w!="run_out"&&w!="obstructing_field"&&w!="retired_hurt")bowl["wickets"]=(int)bowl["wickets"]+1;const char *out=e["dismissedPlayerId"]|e["batterId"].as<const char*>();s["batters"][out]["out"]=true;s["batters"][out]["dismissal"]=w;JsonArray fow=s["fow"].as<JsonArray>();JsonObject fall=fow.add<JsonObject>();fall["score"]=total;fall["wicket"]=s["wickets"];fall["player"]=s["batters"][out]["name"];fall["over"]=String(legalBefore/6)+"."+String(legalBefore%6);if(e["replacementPlayerId"].is<const char*>()&&s["wickets"].as<int>()<10){String rep=e["replacementPlayerId"].as<String>();if(s["striker"].as<String>()==out)s["striker"]=rep;else if(s["nonStriker"].as<String>()==out)s["nonStriker"]=rep;}}
  if(legal&&((int)s["legalBalls"]%6)==0){String a=s["striker"].as<String>();s["striker"]=s["nonStriker"].as<String>();s["nonStriker"]=a;}
  if((kind=="runs"||kind=="wicket"||kind=="wide"||kind=="no_ball"||kind=="bye"||kind=="leg_bye")&&(runs%2)){String a=s["striker"].as<String>();s["striker"]=s["nonStriker"].as<String>();s["nonStriker"]=a;}
  JsonArray overs=s["overs"].as<JsonArray>();if(overs.size()==0||legalBefore%6==0){JsonObject overNew=overs.add<JsonObject>();overNew["runs"]=0;overNew["wickets"]=0;overNew["balls"].to<JsonArray>();}JsonObject over=overs[overs.size()-1];int conceded=kind=="wide"||kind=="no_ball"?1+runs:(kind=="bye"||kind=="leg_bye"||kind=="wicket"?0:runs);over["runs"]=(int)over["runs"]+conceded;if(e.containsKey("wicket"))over["wickets"]=(int)over["wickets"]+1;String label=kind=="wide"?"Wd":kind=="no_ball"?"Nb":kind=="bye"?"B":kind=="leg_bye"?"Lb":e.containsKey("wicket")?"W":String(runs);over["balls"].as<JsonArray>().add(label);if(legal&&((int)s["legalBalls"]%6)==0&&(int)over["runs"]==0)bowl["maidens"]=(int)bowl["maidens"]+1;JsonObject cp=s["currentPartnership"].as<JsonObject>();cp["runs"]=(int)cp["runs"]+(total-partnershipStart);if(legal)cp["balls"]=(int)cp["balls"]+1;if(e.containsKey("wicket")){JsonArray parts=s["partnerships"].as<JsonArray>();parts.add(cp);JsonObject next=s["currentPartnership"].to<JsonObject>();next["batters"][0]=s["striker"];next["batters"][1]=s["nonStriker"];next["runs"]=0;next["balls"]=0;}JsonArray progression=s["progression"].as<JsonArray>();JsonObject point=progression.add<JsonObject>();point["legalBalls"]=s["legalBalls"];point["total"]=total;point["wickets"]=s["wickets"];
  s["completed"]=(s["wickets"].as<int>()>=10)||(s["legalBalls"].as<int>()>=(s["maxOvers"].as<int>()*6));
}

bool ScoringEngine::rebuild(JsonArrayConst events, JsonObject state, String &error) {
  for(JsonObjectConst e:events){if(!validate(e,error))return false;apply(e,state);}return true;
}
