#include <Scanner.hpp>
#include <Parser.hpp>
#include <Renamer.hpp>
#include <Scheduler.hpp>
#include <iostream>
#include <cstring>
#include <ctime>

void help () {
   std::cout << "Command Syntax: schedule [-h] [<name>]" << std::endl;
   std::cout << "Options:" << std::endl;
   std::cout << "   -h: Print this help menu." << std::endl;
   std::cout << "   <name>: Invoke schedule on the input ILOC block contained in <name> and output a reordered or scheduled ILOC block." << std::endl;
}

void schedule (std::string filename) {

   try {
      Scanner scanner (filename);

      try {

         Parser parser (scanner);
         InternalRepresentation rep = parser.parse();

         try {

            Renamer renamer;
            renamer.rename(rep);

            Scheduler scheduler;
            Schedule schedule = scheduler.schedule(rep);

            // Print output
            for (const auto& cycle : schedule.cycles) {
               std::cout << "[ " << cycle.first.printVR() << " ; " << cycle.second.printVR() << " ]" << std::endl;
            }
            
         } catch (RenamingFailedException& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
         }
      } catch (ParseFailedException& e) {
         std::cerr << "Due to syntax errors, run terminates." << std::endl;
      }
   } catch (FileNotFoundException& e) {
      std::cerr << "ERROR: " << e.what() << std::endl;
   }
}

int main (int argc, char *argv[]) {

   if (argc < 2) {
      std::cerr << "ERROR: Must provide command line arguments." << std::endl;
      return -1;
   }

   if (!strcmp(argv[1], "-h")){
      help();
   } else {
      schedule(argv[1]);      
   }

   return 0;
}
