/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: gps.h
*/



#ifndef GPS_HEADER
#define GPS_HEADER



#define GPS_DATA_INVALID    (0)
#define GPS_DATA_VALID      (1)
#define GPS_POSITION_NOFIX  (1)
#define GPS_POSITION_2DFIX  (2)
#define GPS_POSITION_3DFIX  (3)



//GPS data from GNSS module (raw)
#define CHAR_RESERVE        (2)
struct gps_raw_struct
{
    char time[sizeof("hhmmss.ss") + CHAR_RESERVE];          //UTC Time
    char date[sizeof("ddmmyy") + CHAR_RESERVE];             //UTC Date
    char latitude[sizeof("ddmm.mmmm") + CHAR_RESERVE];      //Shirota
    char ns[sizeof("x") + CHAR_RESERVE];                    //Northing
    char longitude[sizeof("dddmm.mmmm") + CHAR_RESERVE];    //Dolgota
    char ew[sizeof("x") + CHAR_RESERVE];                    //Easting
    char speed[sizeof("xxx.yyy") + CHAR_RESERVE];           //Speed over ground, knots
    char course[sizeof("xxx.y") + CHAR_RESERVE];            //Course over ground, deg
    char status[sizeof("x") + CHAR_RESERVE];                //A=Valid, V=Invalid
    char altitude[sizeof("aaaaa.a") + CHAR_RESERVE];        //Altitude above means sea level, meters probably
    char sat_view[sizeof("nn") + CHAR_RESERVE];             //Number of SVs used for navigation
    char sat_used[sizeof("nn") + CHAR_RESERVE];             //Number of SVs used for navigation
    char mode[sizeof("x") + CHAR_RESERVE];                  //0=No, 1=2D, 2=3D
    char pdop[sizeof("xx.yy") + CHAR_RESERVE];              //Positional dilution of precision
};



//Similar to (but not the same as) gps_raw_struct, in numerical format
struct gps_num_struct
{
    uint8_t hour;       //UTC Time
    uint8_t minute;
    uint8_t second;

    uint8_t day;        //UTC Date
    uint8_t month;
    uint8_t year;

    struct {
        float in_deg;   //Lat in decimal deg
        double in_rad;  //in radians (converted), scaled to double to improve relative pos calculation accuracy
    } latitude;

    struct {
        float in_deg;   //Lon in decimal deg
        double in_rad;  //in radians (converted), scaled to double to improve relative pos calculation accuracy
    } longitude;

    float speed;        //kilometers per hour (converted)
    float course;       //decimal degrees
    float altitude;     //meters above means sea level, signed

    uint8_t sat_view;
    uint8_t sat_used;

    uint8_t status;     //0=Invalid, 1=Valid
    uint8_t mode;       //1=NoFix, 2=2D, 3=3D
    float pdop;
};



uint8_t parse_gps(void);
char *get_nmea_buf(void);
struct gps_raw_struct *get_gps_raw(void);
struct gps_num_struct *get_gps_num(void);
uint8_t get_gps_status(void);



#endif /*GPS_HEADER*/
