
CMD_NOP        =  0
CMD_INVALID    =  1
CMD_ERROR      =  2
CMD_CHAT       =  3
CMD_PLAYER     =  4
CMD_GAME       =  5
CMD_ADMIN      =  6

PLAYER_JOIN    =  0
MESSAGE        =  1
ERROR          =  2
READY          =  3
NOTREADY       =  4
START_PLACING  =  5
BEGIN          =  6
MOVE           =  7
ATTACK         =  8
PLACE          =  9
GET_ARMIES     = 10
ATTACK_RESULT  = 11
MOVE_RESULT    = 12
GAME_STATUS    = 13
PLAYER_STATUS  = 14
COUNTRY_STATUS = 15
DEFEAT         = 16
VICTORY        = 17
PLAYER_QUIT    = 18

GAMED_ERR_GAME_FULL = 0
GAMED_ERR_NO_GAME   = 1
GAMED_ERR_IN_GAME   = 2

CMD_TALK          = 0
CMD_WHISPER       = 1
CMD_EMOTE         = 2
CMD_ADD_FRIEND    = 3
CMD_FRIEND_STATUS = 4
CMD_REMOVE_FRIEND = 5

CMD_RENAME = 0
CMD_COMMAND  = 1

CMD_LIST_GAMES          = 0
CMD_LIST_GAME_INSTANCES = 1
CMD_CREATE_GAME         = 2
CMD_JOIN_GAME           = 3
CMD_LIST_PLAYERS        = 4
CMD_QUIT_GAME           = 5

CMD_MAP = {
            CMD_NOP: ['NOP'],
            CMD_INVALID: ["Invalid command"],
            CMD_ERROR: ['Game full','No Game','Already in game','Game exists'],
            CMD_CHAT: ['Talk','Whisper','Emote','Add Friend','Friend Status','Remove Friend'],
            CMD_PLAYER: ['Declare client','Rename Player','Get player stats'],
            CMD_GAME: ['List games','List instances','Create Game','Join Game','List Players','Quit Game'],
            CMD_ADMIN: ['Admin'],
}

COUNTRIES = [ 'EASTERN US', 'NORTHWEST TERRITORY', 'WESTERN US', 'ONTARIO',
  'CENTRAL AMERICA', 'ALBERTA', 'GREENLAND', 'ALASKA', 'QUEBEC', 'BRAZIL',
  'VENEZUELA', 'ARGENTINA', 'PERU', 'ICELAND', 'SOUTHERN EUROPE', 'UKRAINE',
  'SCANDINAVIA', 'GREAT BRITAIN', 'WESTERN EUROPE', 'NORTHERN EUROPE', 'EGYPT',
  'CONGO', 'MADAGASCAR', 'SOUTH AFRICA', 'EAST AFRICA', 'NORTH AFRICA',
  'AFGHANISTAN', 'MONGOLIA', 'URAL', 'JAPAN', 'IRKUTSK', 'INDIA', 'SIAM',
  'YAKUTSK', 'SIBERIA', 'CHINA', 'KAMCHATKA', 'MIDDLE EAST', 'NEW GUINEA',
  'INDONESIA', 'WESTERN AUSTRALIA', 'EASTERN AUSTRALIA',
]

ERRORS = ['Invalid command','Not enough players', 'Not enough armies',
  'Do not own country', 'Invalid destination']

COMMANDS = [
  'PLAYER_JOIN', 'MESSAGE', 'ERROR', 'READY', 'NOTREADY',
  'START_PLACING', 'BEGIN', 'MOVE', 'ATTACK', 'PLACE', 'GET_ARMIES',
  'ATTACK_RESULT', 'MOVE_RESULT', 'GAME_STATUS', 'PLAYER_STATUS',
  'COUNTRY_STATUS', 'DEFEAT', 'VICTORY', 'PLAYER_QUIT'] 

