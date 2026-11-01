import type {MatchState} from './engine';
const BASE_KEY='cricscore.device', TOKEN_KEY='cricscore.token', CACHE_KEY='cricscore.last';
export const deviceUrl=()=>localStorage.getItem(BASE_KEY)||'http://192.168.4.1';
export const token=()=>localStorage.getItem(TOKEN_KEY)||'';
export function setConnection(url:string,key:string){localStorage.setItem(BASE_KEY,url.replace(/\/$/,''));localStorage.setItem(TOKEN_KEY,key);}
async function call<T>(path:string,opts:RequestInit={}):Promise<T>{const c=new AbortController(),timer=setTimeout(()=>c.abort(),5000);try{const res=await fetch(`${deviceUrl()}${path}`,{...opts,signal:c.signal,headers:{'Content-Type':'application/json','X-CRICSCORE-TOKEN':token(),...opts.headers}});const d=await res.json();if(!res.ok)throw Error(d.error||`Device error ${res.status}`);return d as T;}finally{clearTimeout(timer)}}
export async function getStatus(){return call<{online:boolean;currentMatchId:string;version:string}>('/api/status');}
export async function getMatches(){const d=await call<{matches:MatchState[]}>('/api/matches');localStorage.setItem(CACHE_KEY,JSON.stringify(d.matches));return d.matches;}
export async function getCurrent():Promise<MatchState|undefined>{try{return await call<MatchState>('/api/match/current')}catch(e){if(e instanceof Error&&e.message==='Match not found')return undefined;throw e;}}
export async function sendEvent(e:Record<string,unknown>){return call('/api/match/event',{method:'POST',body:JSON.stringify(e)});}
export async function undoEvent(){return call('/api/match/undo',{method:'POST',body:'{}'});}
export async function createRemoteMatch(input:Record<string,unknown>){return call<MatchState>('/api/match/create',{method:'POST',body:JSON.stringify(input)});}
export async function startRemoteMatch(input:Record<string,unknown>){return call<MatchState>('/api/match/start',{method:'POST',body:JSON.stringify(input)});}
export async function startSecondInnings(input:Record<string,unknown>){return call<MatchState>('/api/match/next-innings',{method:'POST',body:JSON.stringify(input)});}
export async function controlMatch(action:string,extra:Record<string,unknown>={}){return call<MatchState>('/api/match/control',{method:'POST',body:JSON.stringify({action,...extra})});}
export async function editRemoteEvent(index:number,event:Record<string,unknown>){return call<MatchState>('/api/match/edit',{method:'POST',body:JSON.stringify({index,event})});}
export async function cachedMatches():Promise<MatchState[]>{try{return JSON.parse(localStorage.getItem(CACHE_KEY)||'[]')}catch{return []}}
export async function getDeviceMatch(id:string){return call<MatchState>(`/api/match/${encodeURIComponent(id)}`)}
export async function exportMatch(id:string){return call<MatchState>(`/api/match/${encodeURIComponent(id)}`)}
