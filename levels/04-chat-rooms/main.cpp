#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "room.h"

// Replays a chat transcript. Each line is "<room> <handle> <message...>".
// A message of "/join" only joins; "/mod <handle>" promotes a member when
// sent by a moderator. The first member to join a room becomes its moderator.
// Mentioning someone who is not in the room yet adds them to it.
int main(int argc, char** argv) {
    const char* path = argc > 1 ? argv[1] : "transcript.txt";
    std::ifstream file(path);
    if (!file) {
        std::cerr << "cannot open " << path << "\n";
        return 1;
    }

    std::map<std::string, Room> rooms;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream in(line);
        std::string room_name, handle, text;
        if (!(in >> room_name >> handle)) {
            continue;
        }
        std::getline(in >> std::ws, text);

        Room& room = rooms.try_emplace(room_name, room_name).first->second;
        bool first = room.members().empty();
        Member& member = room.join(handle);
        if (first) {
            member.moderator = true;
        }

        if (text == "/join") {
            continue;
        }
        if (text.rfind("/mod ", 0) == 0) {
            Member* target = room.find(text.substr(5));
            if (member.moderator && target != nullptr) {
                target->moderator = true;
            }
            continue;
        }

        room.post(member, text);
    }

    for (const auto& [name, room] : rooms) {
        std::printf("#%s  (%d messages)\n", name.c_str(), room.total_messages());
        for (const Member& m : room.members()) {
            std::printf("  %-8s sent=%-3d mentions=%-3d%s\n", m.handle.c_str(), m.messages_sent,
                        m.mentions, m.moderator ? " [mod]" : "");
        }
    }
    return 0;
}
