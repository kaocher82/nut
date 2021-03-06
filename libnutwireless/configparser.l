
%x Value
%x StrVal
%x Comment
%option stack
%option noyywrap
%option nounput
%option noyy_top_state

%{
	#include "cconfigparser.h"
	int line_num = 1;
	unsigned char char_buff = 0;
	libnutwireless::CConfigParser* libnutcconfigparser;
	void configparsererror (libnutwireless::CConfigParser *cp, const char* s);

	#include <QHostAddress>
	#include "configparser_yacc.hpp"
	#define yylval configparserlval
	#define YY_INPUT(buf,result,max_size) { result = libnutcconfigparser->readFromStream(buf,max_size); }
%}

%%

network      return NETWORK;
ssid         return SSID;
bssid        return BSSID;
disabled     return DISABLED;
id_str       return ID_STR;
scan_ssid    return SCAN_SSID;
priority     return PRIORITY;
mode         return MODE;
frequency    return FREQUENCY;
proto        return PROTO;
key_mgmt     return KEY_MGMT;
auth_alg     return AUTH_ALG;
pairwise     return PAIRWISE;
group        return GROUP;
psk          return PSK;
eapol_flags  return EAPOL_FLAGS;
mixed_cell   return MIXED_CELL;
proactive_key_caching return PROACTIVE_KEY_CACHING;
wep_key0     return WEP_KEY0;
wep_key1     return WEP_KEY1;
wep_key2     return WEP_KEY2;
wep_key3     return WEP_KEY3;
wep_tx_keyidx return WEP_TX_KEYIDX;
peerkey      return PEERKEY;
eap          return EAP;
identity     return IDENTITY;
anonymous_identity return ANONYMOUS_IDENTITY;
password     return PASSWORD;
ca_cert      return CA_CERT;
ca_path      return CA_PATH;
client_cert  return CLIENT_CERT;
private_key  return PRIVATE_KEY;
private_key_passwd return PRIVATE_KEY_PASSWD;
dh_file      return DH_FILE;
subject_match return SUBJECT_MATCH;
altsubject_match return ALTSUBJECT_MATCH;
phase1       return PHASE1;
phase2       return PHASE2;
ca_cert2     return CA_CERT2;
ca_path2     return CA_PATH2;
client_cert2 return CLIENT_CERT2;
private_key2 return PRIVATE_KEY2;
private_key2_passwd return PRIVATE_KEY2_PASSWD;
dh_file2     return DH_FILE2;
subject_match2 return SUBJECT_MATCH2;
altsubject_match2 return ALTSUBJECT_MATCH2;
fragment_size return FRAGMENT_SIZE;
pac_file     return PAC_FILE;


=\ *(true|TRUE|True)\ *\n {
			yylval.b = true; return BOOL; }

=\ *(false|FALSE|False)\ *\n {
			yylval.b = false; return BOOL; }

=\ *([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2}\ *\n {
            yylval.macAddr = new libnutcommon::MacAddress(QString(yytext)); return MACADDR; }

=\ *(-[[:blank:]]*)?[[:digit:]]+\ *\n {
            yylval.i = atoi(&yytext[1]); return INTEGER; }

=\ *        yylval.str = new QString(); BEGIN(Value);
=\ *\"      yylval.str = new QString("\""); BEGIN(StrVal);
#.*\n       printf("found a comment in #");
"/*"       yy_push_state(Comment);

\n         ++line_num; return yytext[0];
"//".*\n   ++line_num; return yytext[0];
[a-zA-Z0-9]+ {
	char_buff = yytext[yyleng];
	printf("Unexpected keyword(%i):\n",line_num);
	printf("%s\n",yytext);
	yytext[yyleng] = char_buff;
}
[!{};/]     return yytext[0];
.       {
	char_buff = yytext[yyleng];
	printf("Unexpected keyword(%i):\n",line_num);
	printf("%s\n",yytext);
	yytext[yyleng] = char_buff;
}

<Value>{
	\ *\n       { BEGIN(INITIAL); return VALUE; }
	[^\n\\]*    { yylval.str->append(yytext); }
	\\.         { yylval.str->append(yytext[1]); }
}

<StrVal>{
	\"          { BEGIN(INITIAL); yylval.str->append("\""); return STRVAL; }
	[^\\\"\n]*  { yylval.str->append(yytext); }
	\n          { configparsererror(libnutcconfigparser, "Unexpected newline in string, ending string\n");
	              ++line_num;
	              BEGIN(INITIAL); yylval.str->append("\""); return STRVAL;
	            }
	\\n         { yylval.str->append("\n"); }
	\\\"        { yylval.str->append("\""); }
	\\.         { yylval.str->append(yytext[1]); }
}

	/* comments may be nested */
<Comment>{
	[^/*\n]*        /* eat anything that's not a '*', '/' or newline */
	"*"+[^*/\n]*   /* eat up '*'s not followed by '/'s */
	"*"+"/"        yy_pop_state();
	"/"+[^*/\n]*
	"/"+"*"        yy_push_state(Comment);
	\n             ++line_num;
}

%%
