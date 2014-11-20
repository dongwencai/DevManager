/*
 * =====================================================================================
 *
 *       Filename:  stringex.h
 *
 *    Description:  none
 *
 *        Version:  1.0
 *        Created:  2014年11月10日 18时50分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

char *strip_illegal_char(char *str)
{
    char *tmp = str;
    while(*str)
    {
        if(*str < 32 || *str > 127)
        {
            *str = '\0';
            break;
        }
        str ++;
    }
    return tmp;
}
