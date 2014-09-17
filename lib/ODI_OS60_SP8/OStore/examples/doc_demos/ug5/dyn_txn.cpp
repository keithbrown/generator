// dyn_txn.cpp: uses a dynamic transaction to read notes
// and their priorities into database
#include "note.hh"

note* head; // head of linked-list of notes

int main(int argc, char** argv)
{
	char buff[BUFSIZE], buff2[BUFSIZE];
	int note_priority;

	OS_ESTABLISH_FAULT_HANDLER {
		if(argc!=2) { // check for name of database
			cout << "Usage:  " << argv[0] << " <database>" << endl;
			return 1;
		}

		objectstore::initialize();
		// open database; create one if it doesn't exist
		os_database *db = os_database::open(argv[1], 0, 0644);
	
		// start transaction
		os_transaction* txn = os_transaction::begin(os_transaction::update);

		os_database_root *root_head = db->find_root("head");
		if(!root_head) // if there's no root, create one
			root_head = db->create_root("head");
		head = (note *)root_head->get_value(note::get_os_typespec());

loop:
		cout << "Enter a new note: " << flush; // Prompt for new note
		cin.getline(buff, sizeof(buff));

		cout << "Enter a note priority: " << flush; // Prompt for priority
		cin.getline(buff2, sizeof(buff2));
		note_priority = atoi(buff2);

		// allocate note object and make it the new entry point
		head = new(db, note::get_os_typespec()) note(buff, head, note_priority);
		root_head->set_value(head, note::get_os_typespec());

		cout << "Do you want to commit, write another note, or abort";
		cout << " (c/w/a)? " << flush;
		cin.getline(buff, sizeof(buff));
		if (buff[0] == 'c')
			txn->commit(); // commit transaction
		else if (buff[0] == 'a')
			txn->abort(); // abort transaction
		else
			goto loop; // get another note

		delete txn; // avoid memory leak
		db->close(); // close database
		objectstore::shutdown();

	} OS_END_FAULT_HANDLER
	return 0;
}

