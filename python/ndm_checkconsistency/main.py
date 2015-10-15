
# -------------------------------------------------------------------------------------
#    Helper functions
# -------------------------------------------------------------------------------------

def CommonPrefix(items):
	"""Determine the common prefix for a list of files/directories"""
	if len(items) == 1:
		common = re.sub('-[^-]*$','-',items[0])
	else:
		common = re.sub('-[^-]*$','-',items[0])
		for next in items[1:]:
			i = 0
			while i < len(common) and i < len(next):
				if common[i] != next[i]:
					break
				i = i+1
			common = common[:i]
	return common

def ListSuffixes(directories):
	"""Guess suffixes for a list of directories"""
	common = CommonPrefix(directories)
	suffixes = [re.sub('^'+common,'',directory) for directory in directories]
	return suffixes

def GuessDate(directory):
	"""Guess date from directory name"""
	return re.sub('.*([0-9]{8}).*',r'\1',directory)

def CountFiles(base,session,suffix,extension):
	"""Check that a given file (or list of files) exists"""
	if extension == 'ncs' or extension == 'mpg' or extension == 'smi':
		nnn = r'-[0-9]*'
	else:
		nnn = ''
	directories = fnmatch.filter(os.listdir('.'),base+'-'+suffix)
	files = []
	for directory in directories:
		files.extend(os.listdir(directory))
	files = fnmatch.filter(files,session+'-'+suffix+nnn+'.'+extension)
	return len(files)

# -------------------------------------------------------------------------------------
#    File info
# -------------------------------------------------------------------------------------

class FileInfo():
	def __init__(self,directories):
		self.directories = directories
		self.base = re.sub(r'-$','',CommonPrefix(directories))
		# Mode
		l = fnmatch.filter(os.listdir(directories[0]),'*.smr')
		if len(l) == 0:
			self.mode = 'neuralynx'
		else:
			self.mode = 'spike2'
		# Complete file list (across directories)
		files = []
		for d in directories:
			files.extend(os.listdir(d))
		# Complete session list (across directories)
		self.sessions = [re.sub(r'-[0-9]*.((ncs|mpg|smi))',r'.\1',x) for x in files]
		self.sessions = [re.sub(r'(.*)[.][^.]*',r'\1',x) for x in self.sessions]
		self.suffixes = ListSuffixes(directories)
		for suffix in self.suffixes:
			self.sessions = [re.sub(r'-'+suffix+'$','',x) for x in self.sessions]
		self.sessions = list(set(self.sessions))
		self.sessions.sort()
		# Loop through sessions
		self.items = []
		for session in self.sessions:
			if self.mode == 'neuralynx':
				for suffix in self.suffixes:
					for extension in ['ncs','nvt','nev','mpg','smi']:
						item = []
						item.append(session)
						item.append(suffix)
						item.append(extension)
						item.append(CountFiles(self.base,session,suffix,extension))
						self.items.append(item)
			elif self.mode == 'spike2':
				item = []
				item.append(session)
				item.append(suffix)
				item.append('avi')
				item.append(CountFiles(self.base,session,'*','avi'))
				self.items.append(item)
				for suffix in self.suffixes:
					item = []
					item.append(session)
					item.append(suffix)
					item.append('smr')
					item.append(CountFiles(self.base,session,suffix,'smr'))
					self.items.append(item)

# -------------------------------------------------------------------------------------
#    Info dialog
# -------------------------------------------------------------------------------------

class InfoDialog(QtGui.QDialog):
	def __init__(self,info):
		"""Initialize variables and dialog items"""
		QtGui.QDialog.__init__(self)
		# Create input dialog
		self.ui = Ui_InfoDialog()
		self.ui.setupUi(self)
		self.ui.list.setRowCount(len(info.items))
		for row in range(0,len(info.items)):
			extension = info.items[row][2]
			n = info.items[row][3]
			for column in range(0,4):
				if column == 3 and (extension != 'ncs' and extension != 'mpg' and extension != 'smi'):
					if n == 0:
						icon = QtGui.QIcon(':/images/cancel.png')
					else:
						icon = QtGui.QIcon(':/images/ok.png')
					cell = QtGui.QTableWidgetItem()
					cell.setIcon(icon)
				else:
					cell = QtGui.QTableWidgetItem(str(info.items[row][column]))
					if column == 2:
						#if extension == 'mpg' or extension == 'smi' or extension == 'nvt':
						if extension in ['mpg','smi','nvt','avi']:
							icon = QtGui.QIcon(':/images/video.png')
						else:
							icon = QtGui.QIcon(':/images/binary.png')
						cell.setIcon(icon)
				cell.setFlags(QtCore.Qt.ItemIsEnabled)
				self.ui.list.setItem(row,column,cell)
		for column in range(0,4):
			self.ui.list.resizeColumnToContents(column)
		self.ui.list.horizontalHeader().setResizeMode(0,QtGui.QHeaderView.Stretch)
		self.ui.list.verticalHeader().hide()
		for row in range(0,len(info.items)):
			self.ui.list.resizeRowToContents(row)

# -------------------------------------------------------------------------------------
#    Start here
# -------------------------------------------------------------------------------------

if __name__ == '__main__':
	import sys
	app = QtGui.QApplication(sys.argv)

	# Get directory list (either graphically, or from the command-line)
	directories = [re.sub(r'/$','',x) for x in sys.argv[1:]]
	if len(directories) == 0:
		dialog = QtGui.QFileDialog()
		dialog.setFileMode(QtGui.QFileDialog.DirectoryOnly)
		dialog.findChild(QtGui.QListView,'listView').setSelectionMode(QtGui.QAbstractItemView.MultiSelection)
		dialog.setWindowTitle('Choose one or more directories to check')
		if dialog.exec_():
			d = [re.sub(r'/$','',str(x)) for x in list(dialog.selectedFiles())]
			here = dialog.directory().path()
			for directory in d:
				if directory != here:
					directories.append(directory)
		else:
			sys.exit(0)

	# Check directory list
	path = os.path.dirname(directories[0])
	for directory in directories[1:]:
		if os.path.dirname(directory) != path:
			box = QtGui.QMessageBox()
			box.setText('Data directories should have the same parent directory.')
			box.setIcon(QtGui.QMessageBox.Critical)
			box.exec_()
			sys.exit(1)
	# Make directory list relative and chdir to parent directory
	directories = [os.path.basename(x) for x in directories]
	if path != '':
		os.chdir(path)

	# List all files
	info = FileInfo(directories)
	dialog = InfoDialog(info)
	result = dialog.exec_()
	sys.exit(0)

