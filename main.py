"main.py"

from src import constants as C;
from src import types as T;


def main() -> None:
	forum:T.Forum = T.Forum("testForum0");
	
	#Try adding posts & folders.
	forum.post("First post!", "Testing out the forum class system.", 0);
	folder:T.Folder = forum.mkdir("Miscellaneous", 0);
	post:T.Post = folder.post("Nested post", "Is inside a folder!", 0);
	post.reply(0, "Very neat!");

	forum.saveToDisk();


if (__name__ == "__main__"): main();