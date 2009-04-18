/*
 * json.h
 *
 *  Created on: Oct 13, 2008
 *      Author: aurich
 */

#ifndef JSON_H_
#define JSON_H_

char* makeJSON(const void *data, uint32_t tsize, uint8_t start, uint32_t *setme_size);
char* makeChangeUpSpeedJSON(uint8_t tID, uint32_t upspeed, uint32_t *setme_size);
const char* parseResponse(const char* response);
int8_t getTorrentID(const char* response);


#endif /* JSON_H_ */
