/**
 * Created by mike on 1/17/16.
 * Contains functions used to serialize and deserialize packets
 * @author Mike McGee
 */

#ifndef OPENMEMDB_SERIALIZATION_HELPER_H
#define OPENMEMDB_SERIALIZATION_HELPER_H

#include "util/packets.h"

/****************************************************************
 * Serialization functions
 ****************************************************************/

/**
 * Converts CommandPacket into char* that can be sent over socket
 * @param packet The CommandPacket to be serialized
 * @return a char[] representing the packet
 */
char* SerializeCommandPacket(CommandPacket packet);

/**
 * Converts ConnectionPacket into char* that can be sent over socket
 * @param packet The ConnectionPacket to serialize
 * @return a char[] representing the packet
 */
char* SerializeConnectionPacket(ConnectionPacket packet);

/**
 * Converts ResultMetaDataPacket into char* that can be sent over socket
 * @param packet The ResultMetaDataPacket to serialize
 * @return a char[] representing the packet
 */
char* SerializeResultMetaDataPacket(ResultMetaDataPacket packet);

/**
 * Converts ResultPacket into char* that can be sent over socket
 * @param packet The ResultPacket to serialize
 * @return a char[] representing the packet
 */
char* SerializeResultPacket(ResultPacket packet);

/*****************************************************************
 * Deserialization functions
 *****************************************************************/

/**
 * Converts char* into CommandPacket
 * @param serializedPacket The char* representing the CommandPacket
 * @return A CommandPacket with the data from the serializedPacket
 */
CommandPacket DeserializeCommandPacket(char* serializedPacket);

/**
 * Converts char* into ConnectionPacket
 * @param serializedPacket The char* representing the ConnectionPacket
 * @return A ConnectionPacket with the data from the serializedPacket
 */
ConnectionPacket DeserializeConnectionPacket(const char* serializedPacket);

/**
 * Converts char* to ResultMetaDataPacket
 * @param serializedPacker The char* representing the ResultMetaDataPacket
 * @return A ResultMetaDataPacket with the data from the serializedPacket
 */
ResultMetaDataPacket DeserializeResultMetaDataPacket(char* serializedPacket);

/**
 * Converts char* to a ResultPacket
 * @param serializedPacket The char* representing the ResultPacket
 * @return A ResultPacket with the data from the serializedPacket
 */
ResultPacket DeserializeResultPacket(char* serializedPacket);
#endif //OPENMEMDB_SERIALIZATION_HELPER_H
