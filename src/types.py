"types.py"

if (__name__ == "__main__"):
	#Debug
	import constants as C;

else:
	from src import constants as C;

from dataclasses import dataclass;
from enum import Enum;
import os;


class UserType(Enum):
	INVALID = -1; #Invalid user.
	VIEWER = 0; #Only view.
	AUTHOR = 1; #Can add to the posts.
	ADMIN = 2; #Can add to, or remove posts.


@dataclass
class Reply:
	userID:int;
	message:str;


class Post:
	def __init__(self, title:str, body:str, owner:int):
		self.title:str = title;
		self.name:str = title.lower().strip().replace(" ", "-");
		self.body:str = body;
		self.owner:int = owner;
		self.replies:list[Reply] = [];

	def reply(self, userID:int, message:str) -> None:
		self.replies.append(Reply(userID, message));


	def saveToDisk(self, path:str) -> None:
		#Save to disk.
		newPath:str = path + f"/{self.name}";

		#Make folder and write .meta file
		try: os.mkdir(newPath);
		except FileExistsError: pass;

		with open(f"{newPath}/.meta", "w") as metaFile:
			metaFile.write(str(self.owner));
		with open(f"{newPath}/.content", "w") as contentFile:
			contentFile.write(f"{self.title}\n{self.body}");

		for (i, reply) in enumerate(self.replies):
			with open(f"{newPath}/{i}.reply", "w") as replyFile:
				replyFile.write(f"{reply.userID}|{reply.message}");


class Folder:
	def __init__(self, name:str, owner:int):
		self.owner:int = owner; #Who created the folder
		self.name:str = name;
		self.users:dict[int, UserType] = {owner:UserType.ADMIN,}; #Who can view, add to or edit the contents
		self.children:list[Folder|Post] = []; #Children. Can contain more folders, or posts.

	def updateUser(self, userID:int, type:UserType) -> None:
		match (type):
			#Valid types.
			case UserType.VIEWER | UserType.AUTHOR | UserType.ADMIN:
				self.users[userID] = type;

			#Not valid.
			case _:
				raise ValueError(f"Cannot add user of unknown type: {type}");


	def mkdir(self, folderName:str, owner:int):
		self.children.append(Folder(folderName, owner));
		if (owner not in self.users): self.users[owner] = UserType.AUTHOR;
		return self.children[-1];

	def post(self, postTitle:str, postBody:str, owner:int) -> Post:
		self.children.append(Post(postTitle, postBody, owner));
		if (owner not in self.users): self.users[owner] = UserType.AUTHOR;
		return self.children[-1];


	def saveToDisk(self, path:str) -> None:
		#Save to disk.
		newPath:str = path + f"/{self.name}";

		#Make folder and write .meta file
		try: os.mkdir(newPath);
		except FileExistsError: pass;

		with open(f"{newPath}/.meta", "w") as metaFile:
			usersData:list[str] = [];
			#Make string rep of users.
			for (user, type) in self.users.items():
				usersData.append(f"{user}:{type}");
			metaFile.write(f"{self.owner}|{'|'.join(usersData)}");

		for child in self.children:
			child.saveToDisk(newPath);



class Forum:
	def __init__(self, name:str):
		self.children:list[Folder|Post] = []; #Folders & Posts on this forum. Like file directories.
		self.users:dict[int, UserType] = {};
		self.name:str = name;


	def mkdir(self, folderName:str, owner:int) -> Folder:
		self.children.append(Folder(folderName, owner));
		if (owner not in self.users): self.users[owner] = UserType.AUTHOR;
		return self.children[-1];

	def post(self, postTitle:str, postBody:str, owner:int) -> Post:
		self.children.append(Post(postTitle, postBody, owner));
		if (owner not in self.users): self.users[owner] = UserType.AUTHOR;
		return self.children[-1];


	def saveToDisk(self) -> bool: #Success
		path:str = f"forums/{self.name}";

		#Make folder and write .meta file
		try: os.mkdir(path);
		except FileExistsError: pass;

		with open(f"{path}/.meta", "w") as metaFile:
			usersData:list[str] = [];
			#Make string rep of users.
			for (user, type) in self.users.items():
				usersData.append(f"{user}:{type}");
			metaFile.write("|".join(usersData));

		for child in self.children:
			child.saveToDisk(path);



def loadForumFromDisk(name:str) -> Forum:
	pass; #TBA

