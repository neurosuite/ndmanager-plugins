
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
	suffixes = [re.sub('^'+re.escape(common),'',directory) for directory in directories]
	return suffixes

def GuessDate(directory):
	"""Guess date from directory name"""
	pattern1 = '.*([0-9]{8}).*'
	pattern2 = '.*([0-9]{4})-([0-9]{2})-([0-9]{2}).*'
	m = re.match(pattern1,directory)
	if m != None:
		return re.sub(pattern1,r'\1',directory)
	else:
		m = re.match(pattern2,directory)
		if m != None:
			return re.sub(pattern2,r'\1\2\3',directory)
		today = datetime.date.today()
		return str(today.year)+str(today.month).zfill(2)+str(today.day).zfill(2)

# -------------------------------------------------------------------------------------
#    Input data
# -------------------------------------------------------------------------------------

class InputData():
	def __init__(self,directories):
		# Mode
		l = fnmatch.filter(os.listdir(directories[0]),'*.smr')
		if len(l) == 0:
			self.mode = 'neuralynx'
		else:
			self.mode = 'spike2'
		self.directories = directories
		self.date = GuessDate(directories[0])
		self.descriptions = []
		self.name = re.sub('(_|-)$','',CommonPrefix(directories))
		if self.mode == 'spike2':
			self.nSessions = -1
			for directory in directories:
				files = os.listdir(directory)
				files = [re.sub('([0-9])-1.avi$',r'\1',file) for file in files]
				files = [re.sub('[.][^.]*$','',file) for file in files]
				sessions = list(set(files))
				if self.nSessions == -1:
					self.nSessions = len(sessions)
				elif self.nSessions != len(sessions):
					self.nSessions = -1
					break
			self.suffixes = ListSuffixes(directories)
			self.nSuffixes = len(self.suffixes)
		elif self.mode == 'neuralynx':
			self.nSessions = len(directories)
			self.suffixes = ['nlx']
			self.nSuffixes = 1

# -------------------------------------------------------------------------------------
#    Input dialog
# -------------------------------------------------------------------------------------

class InputDialog(QtGui.QDialog):
	def __init__(self,data):
		"""Initialize variables and dialog items"""
		QtGui.QDialog.__init__(self)
		# Create input dialog
		self.ui = Ui_InputDialog()
		self.ui.setupUi(self)
		date = QtCore.QDate(int(data.date[0:4]),int(data.date[4:6]),int(data.date[6:8]))
		self.ui.date.setSelectedDate(date)
		self.ui.descriptions.setRowCount(data.nSessions)
		for row in range(0,data.nSessions):
			cell = QtGui.QTableWidgetItem('')
			self.ui.descriptions.setItem(row,0,cell)
			self.ui.descriptions.resizeRowToContents(row)
		self.ui.descriptions.horizontalHeader().setStretchLastSection(True)
		self.ui.suffixes.setRowCount(data.nSuffixes)
		for row in range(0,data.nSuffixes):
			cell = QtGui.QTableWidgetItem(data.suffixes[row])
			self.ui.suffixes.setItem(row,0,cell)
			self.ui.suffixes.resizeRowToContents(row)
		self.ui.suffixes.horizontalHeader().setStretchLastSection(True)
		self.ui.name.setText(data.name)
		if data.mode == 'neuralynx':
			self.ui.system.setText('Neuralynx')
		elif data.mode == 'spike2':
			self.ui.system.setText('CED Spike2')
	def done(self,result):
		if result == QtGui.QDialog.Accepted:
			if self.ui.name.text() == '':
				box = QtGui.QMessageBox()
				box.setText('Missing name.')
				box.setIcon(QtGui.QMessageBox.Critical)
				box.exec_()
			else:
				for row in range(0,self.ui.descriptions.rowCount()):
					if str(self.ui.descriptions.item(0,row).text()) == '':
						box = QtGui.QMessageBox()
						box.setText('Missing descriptions.')
						box.setIcon(QtGui.QMessageBox.Critical)
						box.exec_()
						return
				for row in range(0,self.ui.suffixes.rowCount()):
					if str(self.ui.suffixes.item(0,row).text()) == '':
						box = QtGui.QMessageBox()
						box.setText('Missing suffixes.')
						box.setIcon(QtGui.QMessageBox.Critical)
						box.exec_()
						return
				QtGui.QDialog.done(self,result)
		else:
			QtGui.QDialog.done(self,result)
	def getData(self,data):
		"""Read variables from dialog items"""
		data.name = str(self.ui.name.text())
		data.date = re.sub('-','',str(self.ui.date.selectedDate().toString(QtCore.Qt.ISODate)))
		data.descriptions = []
		for row in range(0,data.nSessions):
			data.descriptions.append(str(self.ui.descriptions.item(0,row).text()))
		data.suffixes = []
		for row in range(0,data.nSuffixes):
			data.suffixes.append(str(self.ui.suffixes.item(0,row).text()))

# -------------------------------------------------------------------------------------
#    Rename dialog
# -------------------------------------------------------------------------------------

class RenameDialog(QtGui.QDialog):
	def __init__(self,renamer):
		"""Initialize variables and dialog items"""
		QtGui.QDialog.__init__(self)
		# Create input dialog
		self.ui = Ui_RenameDialog()
		self.ui.setupUi(self)
		self.ui.names.setRowCount(len(renamer.currentNames))
		for row in range(0,len(renamer.currentNames)):
			name = renamer.currentNames[row]
			cell = QtGui.QTableWidgetItem(name)
			cell.setFlags(QtCore.Qt.ItemIsEnabled)
			extension = re.sub(r'.*[.]([^.]*)',r'\1',name)
			if extension in ['mpg','smi','nvt','avi']:
				icon = QtGui.QIcon(':/images/video.png')
			else:
				icon = QtGui.QIcon(':/images/binary.png')
			cell.setIcon(icon)
			self.ui.names.setItem(row,0,cell)
			cell = QtGui.QTableWidgetItem(renamer.newNames[row])
			cell.setFlags(QtCore.Qt.ItemIsEnabled)
			cell.setIcon(icon)
			self.ui.names.setItem(row,1,cell)
		self.ui.names.resizeColumnToContents(0)
		self.ui.names.resizeColumnToContents(1)
		self.ui.names.verticalHeader().hide()
		self.ui.names.horizontalHeader().setResizeMode(0,QtGui.QHeaderView.Stretch)
		for row in range(0,len(renamer.currentNames)):
			self.ui.names.resizeRowToContents(row)

# -------------------------------------------------------------------------------------
#    Rename files
# -------------------------------------------------------------------------------------

class BatchRenamer():
	def __init__(self,data):
		self.currentDirectories = data.directories
		if data.mode == 'neuralynx':
			# Loop through subdirectories
			n = 0
			self.currentNames = []
			self.newNames = []
			self.newDirectories = [data.name + '-' + data.date + '-' + data.suffixes[0]]
			for directory in data.directories:
				currentNames = sorted(os.listdir(directory))
				newNames = []
				# Determine target base name
				n = n+1
				nn = str(n).zfill(2)
				base = data.name + '-' + data.date + '-' + nn + '-' + data.descriptions[n-1] + '-' + data.suffixes[0]
				# Loop through files
				for name in currentNames:
					newName = re.sub('^CSC([0-9])[.]ncs',base+r'-00\1.ncs',name,re.IGNORECASE)
					newName = re.sub('^CSC([0-9]{2})[.]ncs',base+r'-0\1.ncs',newName,re.IGNORECASE)
					newName = re.sub('^CSC([0-9]{3})[.]ncs',base+r'-\1.ncs',newName,re.IGNORECASE)
					newName = re.sub('^VT1[.](mpg|smi)$',base+r'-00.\1',newName,re.IGNORECASE)
					newName = re.sub('^VT1_([0-9]*[.][^.]*)$',base+r'-\1',newName,re.IGNORECASE)
					newName = re.sub('^(VT1)[.](nvt)$',base+r'.\2',newName,re.IGNORECASE)
					newName = re.sub('^VT1[.]nvt$',base+r'.nvt',newName,re.IGNORECASE)
					newName = re.sub('^Events[.]nev$',base+r'.nev',newName,re.IGNORECASE)
					newName = re.sub('^CheetahLogFile[.]txt$',base+r'.log',newName,re.IGNORECASE)
					newNames.append(newName)
				self.currentNames.extend([directory+'/'+f for f in currentNames])
				self.newNames.extend([self.newDirectories[0]+'/'+f for f in newNames])
		elif data.mode == 'spike2':
			# Loop through subdirectories
			self.currentNames = []
			self.newNames = []
			self.newDirectories = []
			s = 0
			for directory in data.directories:
				s = s+1
				n = 0
				sessions = [re.sub('([0-9])-1.avi$',r'\1',x) for x in os.listdir(directory)]
				sessions = [re.sub('[.][^.]*$','',x) for x in sessions]
				sessions = sorted(list(set(sessions)))
				# Loop through sessions
				for session in sessions:
					n = n+1
					currentNames = fnmatch.filter(os.listdir(directory),session+'*')
					newNames = []
					# Loop through files
					for name in currentNames:
						# Determine target base name
						nn = str(n).zfill(2)
						target = data.name + '-' + data.date + '-' + data.suffixes[s-1]
						base = data.name + '-' + data.date + '-' + nn + '-' + data.descriptions[n-1] + '-' + data.suffixes[s-1]
						newName = re.sub('^.*[.]',base+r'.',name.lower(),re.IGNORECASE)
						newName = re.sub('-1.avi',r'.avi',newName,re.IGNORECASE)
						newNames.append(newName)
					# Add current and new names to stored lists
					self.currentNames.extend([directory+'/'+f for f in currentNames])
					self.newNames.extend([target+'/'+f for f in newNames])
					self.newDirectories.append(target)
			self.newDirectories = list(set(self.newDirectories))
	def rename(self):
		# Create new directories
		for d in self.newDirectories:
			os.mkdir(d)
		# Rename files
		for i in range(0,len(self.newNames)):
			os.rename(self.currentNames[i],self.newNames[i])
		# Remove old directories
		for d in self.currentDirectories:
			os.rmdir(d)


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
		dialog.setWindowTitle('Choose one or more directories to prepare')
		if dialog.exec_():
			d = [re.sub(r'/$','',str(x)) for x in list(dialog.selectedFiles())]
			here = dialog.directory().path()
			for directory in d:
				if directory != here:
					directories.append(directory)
		else:
			sys.exit(0)

	# Remove duplicates and sort alphabetically
	directories = list(set(directories))
	directories.sort()

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

	# Setup session data and display input dialog
	data = InputData(directories)
	if data.nSessions == -1:
		box = QtGui.QMessageBox()
		box.setText('These directories contain different numbers of sessions.')
		box.setIcon(QtGui.QMessageBox.Critical)
		box.exec_()
		sys.exit(1)
	dialog = InputDialog(data)
	result = dialog.exec_()

	# Rename files
	if result == QtGui.QDialog.Rejected:
		sys.exit(0)
	dialog.getData(data)
	renamer = BatchRenamer(data)
	dialog = RenameDialog(renamer)
	result = dialog.exec_()
	if result == QtGui.QDialog.Rejected:
		sys.exit(0)
	exit
	renamer.rename()

	# Display final info
	command = 'ndm_checkconsistency '
	for directory in renamer.newDirectories:
		command = command + ' ' + directory
	print command
	os.system(command)
	sys.exit(0)
