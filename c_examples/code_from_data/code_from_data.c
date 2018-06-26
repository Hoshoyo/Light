#include <stdio.h>

typedef struct XMLTag{
    char*  name;
    int    name_length;
    struct XMLTag* subtags;
} XMLTag;

void parse_xml(char* data, int length) {

}

int main() {
    return 0;
}

/*
    
<response>
    <returncode>SUCCESS</returncode>
    <meetingID>random-7229358</meetingID>
    <internalMeetingID>f8a044c6997b3d8bd92077c8bc63263299d40b92-1530023764628</internalMeetingID>
    <parentMeetingID>bbb-none</parentMeetingID>
    <attendeePW>ap</attendeePW>
    <moderatorPW>mp</moderatorPW>
    <createTime>1530023764628</createTime>
    <voiceBridge>78873</voiceBridge>
    <dialNumber>613-555-1234</dialNumber>
    <createDate>Tue Jun 26 16:36:04 CEST 2018</createDate>
    <hasUserJoined>false</hasUserJoined>
    <duration>0</duration>
    <hasBeenForciblyEnded>false</hasBeenForciblyEnded>
    <messageKey></messageKey>
    <message></message>
</response>

type CreateResponse struct {
    Returncode string
    MeetingID  string
    InternalMeetingID string
    ParentMeetingID string
    AttendeePW string
    ModeratorPW string
    CreateTime string
    VoiceBridge string
    DialNumber string
    CreateDate string
    HasUserJoined string
    Duration string
    HasBeenForciblyEnded string
    MessageKey string
    Message string
}

*/