% spots2pos - Extract position samples from .spots files.
%
%  Extract position samples from one or more .spots files, separate or merge
%  multiple LEDs, interpolate missing samples, synchronize, resample, save to
%  .pos file(s) and concatenate.
%
%  USAGE
%
%    spots2pos(filename,<options>)
%
%    filename       either a single spots file, or a concatenation event file
%                   (.cat.evt) to process and concatenate multiple spots files
%                   belonging to the same set
%    <options>      optional list of property-value pairs (see table below)
%
%    =========================================================================
%     Properties    Values
%    -------------------------------------------------------------------------
%     'input'       input frequency (default = 25 Hz)
%     'output'      output frequency (default = 39.0625 Hz)
%     'resolution'  [maxX maxY] video resolution (default = [320 240])
%     'threshold'   luminance threshold (default = 0)
%     'gap'         missing frames are filled with empty frames, unless the
%                   time gap exceeds this threshold (default = 10 s)
%     'leds'        1 or 2 (default = 1)
%     'shift'       corrective time shift for positions (default = 0 s)
%    =========================================================================
%
%  NOTES
%
%    Typical values for 'resolution' include [320 240] and [720 576].
%
%    The first (typically front) LED is shown in red, the second in green.
%
%  CUSTOM DEFAULTS
%
%    If <a href="http://fmatoolbox.sourceforge.net">FMAToolbox</a> is installed, all properties can have custom default values
%    (type 'help <a href="matlab:help CustomDefaults">CustomDefaults</a>' for details).

% Copyright (C) 2004-2012 by Michaël Zugaro, 2004 by Ken Harris
%
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 3 of the License, or
% (at your option) any later version.

function spots2pos(filename,varargin)

% Check number of parameters
if nargin < 1,
	error('Incorrect number of parameters (type ''help spots2pos'' for details).');
end

% Default values
if exist('GetCustomDefaults.m'),
	inputFrequency = GetCustomDefaults('input',25);
	outputFrequency = GetCustomDefaults('output',39.0625);
	threshold = GetCustomDefaults('threshold',0);
	gap = GetCustomDefaults('gap',10);
	leds = GetCustomDefaults('leds',1);
	resolution = GetCustomDefaults('resolution',[320 240]);
	events = [];
else
	inputFrequency = 25;
	outputFrequency = 39.0625;
	threshold = 0;
	leds = 1;
	resolution = [320 240];
	events = [];
end
shift = 0;

% Is the input file a spots file or an event file?
if ~isempty(regexp(filename,'[.]cat[.]evt$')),
	events = LoadEvents(filename);
	if isempty(events.time), error('Empty event file.'); end
	bases = regexprep(events.description,{'beginning of ','end of '},'');
	bases = {bases{1:2:end}}';
	start = events.time(1:2:end);
	durations = diff(events.time);
	durations = durations(1:2:end);
	[path,filename] = fileparts(filename);
	filename = regexprep(filename,'.cat','.pos');
	if isempty(path), path = '.'; end
else
	[path,filename] = fileparts(filename);
	bases = {filename};
	filename = [filename '.pos'];
	if isempty(path), path = '.'; end
end

% Parse options
for i = 1:2:length(varargin),
	if ~ischar(varargin{i}),
		error(['Parameter ' num2str(i+firstIndex) ' is not a property (type ''help spots2pos'' for details).']);
	end
	switch(lower(varargin{i})),
		case 'input',
			inputFrequency = varargin{i+1};
			if ~isnumeric(inputFrequency) | any(size(inputFrequency)~=1) | inputFrequency <= 0,
				error('Incorrect value for property ''input'' (type ''help spots2pos'' for details).');
			end
		case 'output',
			outputFrequency = varargin{i+1};
			if ~isnumeric(outputFrequency) | any(size(outputFrequency)~=1) | outputFrequency <= 0,
				error('Incorrect value for property ''output'' (type ''help spots2pos'' for details).');
			end
		case 'resolution',
			resolution = varargin{i+1};
			if ~isnumeric(resolution) | length(resolution)~=2 | any(resolution) <= 0,
				error('Incorrect value for property ''resolution'' (type ''help spots2pos'' for details).');
			end
		case 'threshold',
			threshold = varargin{i+1};
			if ~isnumeric(threshold) | any(size(threshold)~=1) | threshold <= 0,
				error('Incorrect value for property ''threshold'' (type ''help spots2pos'' for details).');
			end
		case 'leds',
			leds = varargin{i+1};
			if prod(size(leds)) ~= 1 | ~isnumeric(leds) | leds ~= round(leds) | leds <= 0,
				error('Incorrect value for property ''leds'' (type ''help spots2pos'' for details).');
            end
		case 'shift',
			shift = varargin{i+1};
			if ~isdvector(shift),
				error('Incorrect value for property ''shift'' (type ''help spots2pos'' for details).');
			end
			if length(shift) == 1,
				shift = repmat(shift,length(bases),1);
			elseif length(shift) ~= length(bases),
				error('Incorrect number of time shifts (type ''help spots2pos'' for details).');
			end
		otherwise,
			error(['Unknown property ''' num2str(varargin{i}) ''' (type ''help spots2pos'' for details).']);
	end
end

concatenated = [];

% Process each file
for i = 1:length(bases),
	new = ProcessOne([path '/' bases{i}],inputFrequency,outputFrequency,resolution,threshold,leds,shift(i));
	if isempty(events), continue; end
	% Display info about source and target durations (and # samples)
	nSourceSamples = size(new,1);
	sourceDuration = nSourceSamples/outputFrequency;
	nTargetSamples = round(outputFrequency*durations(i));
	disp(' ');
	disp('File size adjustment for position file');
	disp(['    duration    ' num2str(sourceDuration) ' -> ' num2str(durations(i)) ' (' num2str(durations(i)-sourceDuration) ')']);
	disp(['    # frames    ' int2str(nSourceSamples) ' -> ' int2str(nTargetSamples) ' (' int2str(nTargetSamples-nSourceSamples) ')']);
	if abs(durations(i)-sourceDuration) > 3,
		disp('    *** large shift may indicate a problem ***');
	end
	disp(' ');
	% Concatenate with other .pos files
	if ~isempty(new),
		ok = new(:,1)<=durations(i);
		if ~isempty(concatenated),
			% When concatenating, all sessions must have the same number of LEDs; if not, compensate (fill with -1)
			c = size(concatenated,2);
			n = size(new,2);
			if n < c,
				new = [new -ones(size(new,1),c-n)];
			elseif n > c,
				concatenated = [concatenated -ones(size(concatenated,1),n-c)];
			end
		end
		concatenated = [concatenated;new(ok,1)+start(i) new(ok,2:end)];
	end
end

% Save concatenated positions
if ~isempty(events),
	disp(['Fixing and saving concatenated positions to ''' filename '''...']);
	% Interpolate to regular timestamps (concatenation may have slightly shifted samples)
	d = events.time(end);
	t = (0:1/outputFrequency:d)';
	warning('off','MATLAB:interp1:NaNinY');
	dt = diff(concatenated(:,1));
	bad = find(dt>10/outputFrequency);
	concatenated(bad,2:end) = nan;
	output = interp1(concatenated(:,1),concatenated(:,2:end),t,'linear',-1);
	warning('on','MATLAB:interp1:NaNinY');
	output(~isfinite(output)) = -1;
	dlmwrite([path '/' filename],round(output),'\t');
	disp(' ');
end

% ------------------------------------------------------------------------------------------

function output = ProcessOne(basename,inputFrequency,outputFrequency,resolution,threshold,leds,shift)

nSamplesPerScreen = 100;

twoLEDs = leds == 2;
maxX = resolution(1);
maxY = resolution(2);
resampledPositions = [];

% Has this session already been processed before?
posFile = [basename '.pos'];
if exist(posFile),
	keyin = input(['\nThe file ''' posFile ''' already exists. Do you want to redo it (y/n)? '], 's');
	if ~strcmp(lower(keyin),'y'),
		output = dlmread(posFile,'\t');
		t = (0:1:size(output,1)-1)'/outputFrequency;
		output = [t output];
		return
	end
end

% There can be more than one .spots file for a given session (e.g. with Neuralynx systems),
% in which case we will concatenate them
d = dir([basename '*.spots']);
path = fileparts(basename);

filenames = {d(:).name};

if length(filenames) == 0,
	warning(['No .spots file for session ''' basename '''. Creating empty .pos file.']);
	output = [];
	dlmwrite(posFile,output,'\t');
	return
end

% Load .spots file(s)
spots = [];
nFrames = 0;
for i = 1:length(filenames),
	moreSpots = [];
	n = 0;
	filename = [path '/' filenames{i}];
	if exist(filename),
        disp(['Reading ''' filename '''...']);
		moreSpots = load(filename);
		moreSpots(:,1) = moreSpots(:,1) + 1;
		initialTimestamps = (1:max(moreSpots(:,1)))';
		moreSpots = moreSpots(moreSpots(:,7)>threshold,:);
		n = size(initialTimestamps,1);
	end
	if isempty(moreSpots),
		warning(['Empty spots file ''' filename ''' (or threshold too high)']);
		return
	end
	moreSpots(:,1) = moreSpots(:,1) + nFrames;
	nFrames = nFrames + n;
	spots = [spots;moreSpots];
end

fig = figure;
if exist('Browse.m'),
	Browse(fig,'off');
end

% Optionnally, remove spurious spots
while true,
	plot(spots(:,3),spots(:,4),'.','markersize',5);
	xlim([-5 maxX+5]);ylim([-5 maxY+5]);
	set(gca,'ydir','reverse');
	keyin = input('\nIn the figure window, are there spurious spots you wish to remove (y/n)? ', 's');
	if strcmp(lower(keyin),'y'),
		zoom on;
		input('\nIn the figure window, zoom on the area of interest if necessary, then\nhit ENTER to start drawing a polygon around the points you wish to remove.\n(left click = add polygon point, right click = cancel last polygon point,\nmiddle click = close polygon)... ','s');
		zoom off;
		keep = ~UIInPolygon(spots(:,3),spots(:,4));
		spots = spots(keep,:);
	elseif strcmp(lower(keyin),'n'),
		break;
	end
end
if isempty(spots),
	warning(['No spots left for session ''' basename ''' after removal of spurious spots.']);
	return
end

if twoLEDs,

	% Two LEDs

	% Select front versus rear LED in CrCb color space
	% 1) Automatic sorting using K-means clustering
	fig2 = figure;
	crcb = spots(:,8:9);
	idx = kmeans(crcb,2,'start','uniform');
	m1 = mean(crcb(idx==1,1));
	m2 = mean(crcb(idx==2,1));
	[unused,i] = min([m1 m2]);
	isFrontSpot = idx==i; % By arbitrary convention, smallest cr component is front
	plot(crcb(isFrontSpot,1),crcb(isFrontSpot,2),'.','color',[1 0 0],'markersize',10,'linestyle','none');
	hold on;
	plot(crcb(~isFrontSpot,1),crcb(~isFrontSpot,2),'.','color',[0 1 0],'markersize',10,'linestyle','none');
	xlabel('Cr');
	ylabel('Cb');
	keyin = input('\nIs this separation between putative front and rear lights acceptable (y/n)? ', 's');
	if ~strcmp(lower(keyin),'y'),
		% 2) Manual clustering
		zoom on;
		input('\nIn the figure window, zoom on the area of interest if necessary, then\nhit ENTER to start drawing a polygon around the points corresponding to\nthe front spot (left click = add polygon point, right click = cancel\nlast polygon point, middle click = close polygon)... ','s');
		zoom off;
		isFrontSpot = UIInPolygon(crcb(:,1),crcb(:,2));
	end
	frontSpots = spots(isFrontSpot,:);
	rearSpots = spots(~isFrontSpot,:);
	plot(frontSpots(:,3),frontSpots(:,4),'.','color',[1 0 0],'markersize',10,'linestyle','none');
	hold on;
	plot(rearSpots(:,3),rearSpots(:,4),'.','color',[0 1 0],'markersize',10,'linestyle','none');
	xlim([-5 maxX+5]);ylim([-5 maxY+5]);

	% Construct position matrix
	positions = nan(nFrames,4);
	nFrontSpots = Accumulate(frontSpots(:,1),1,nFrames); % number of spots in each frame
	nFrontSpots(nFrontSpots==0) = nan; % to avoid division by 0 (will be discarded later anyway)
	meanFrontX = Accumulate(frontSpots(:,1),frontSpots(:,3),nFrames)./nFrontSpots; % mean X for each frame
	meanFrontY = Accumulate(frontSpots(:,1),frontSpots(:,4),nFrames)./nFrontSpots;
	goodFrames = nFrontSpots ~= nan;
	positions(goodFrames,1:2) = [meanFrontX(goodFrames) meanFrontY(goodFrames)];

	nRearSpots = Accumulate(rearSpots(:,1),1,nFrames);
	nRearSpots(nRearSpots==0) = nan;
	meanRearX = Accumulate(rearSpots(:,1),rearSpots(:,3),nFrames)./nRearSpots;
	meanRearY = Accumulate(rearSpots(:,1),rearSpots(:,4),nFrames)./nRearSpots;
	goodFrames = goodFrames | ~isnan(nRearSpots);
	positions(goodFrames,3:4) = [meanRearX(goodFrames) meanRearY(goodFrames)];

	close(fig2);
else

	% One LED

	% Discard frames with 0 spots; merge all spots in each frame
	nSpots = Accumulate(spots(:,1),1,nFrames); % number of spots in each frame
	nSpots(nSpots==0) = nan; % to avoid division by 0 (will be discarded later anyway)
	meanX = Accumulate(spots(:,1),spots(:,3),nFrames)./nSpots; % mean X for each frame
	meanY = Accumulate(spots(:,1),spots(:,4),nFrames)./nSpots;

	% Construct position matrix
	goodFrames = ~isnan(nSpots);
	positions = nan(nFrames,2);
	positions(goodFrames,:) = [meanX(goodFrames) meanY(goodFrames)];

end

% Read timestamps from sts file (if it exists) or estimate them using sampling rate
stsFile = [basename '.sts'];
if exist(stsFile),
	disp(' ');
	disp(['Using timestamp file ''' stsFile '''.']);
	sourceTimestamps = load(stsFile);
	sourceTimestamps = sourceTimestamps/1e6;
	l1 = size(positions,1);
	l2 = size(sourceTimestamps,1);
	if l1 < l2,
		warning(['The spots file has fewer frames than the sts file (' int2str(l1) ' vs ' int2str(l2) '). Will align them at the beginning...']);
		sourceTimestamps = sourceTimestamps(1:l1);
	elseif l1 > l2,
		warning(['The spots file has more frames than the sts file (' int2str(l1) ' vs ' int2str(l2) '). Ignoring sts file...']);
		sourceTimestamps = (0:nFrames-1)'/inputFrequency;
	end
else
	sourceTimestamps = (0:nFrames-1)'/inputFrequency;
end
% Reorder position samples if necessary (some acquisition systems sometimes mix up timestamps)
[sourceTimestamps,indices] = sortrows(sourceTimestamps);
positions = positions(indices,:);

% Interpolate missing stretches up to 10 frames long
interpolatedPositions = Clean(positions,10,inf);

% Resample at target frequency
duration = sourceTimestamps(end,1);
targetTimestamps = (0:1/outputFrequency:duration)';
warning('off','MATLAB:interp1:NaNinY');
resampledPositions = interp1(sourceTimestamps,interpolatedPositions,targetTimestamps,'linear',-1);
warning('on','MATLAB:interp1:NaNinY');
resampledPositions(~isfinite(resampledPositions)) = -1;

figure(fig);
hold on;
k = 0;
cleanPositions = resampledPositions(~any(resampledPositions==-1,2),:);
p = [];
p2 = [];
disp(' ');
while ~strcmp(input('Hit ENTER to show the next N samples, or type ''save''+ENTER to save the results... ','s'),'save'),
	k = k+1;
	n1 = (k-1)*nSamplesPerScreen+1;
	n2 = k*nSamplesPerScreen;
	if n2 > length(cleanPositions), break; end
	if ~isempty(p), delete(p); end
	if ~isempty(p2), delete(p2); end
	p = plot(cleanPositions(n1:n2,1),cleanPositions(n1:n2,2),'.','color',[1 0 0],'markersize',10,'linestyle','none');
	if twoLEDs,
		p2 = plot(cleanPositions(n1:n2,3),cleanPositions(n1:n2,4),'.','color',[0 1 0],'markersize',10,'linestyle','none');
	end
	xlim([-5 maxX+5]);ylim([-5 maxY+5]);
end
close(fig);

if ~isempty(spots),
	disp(['Saving to ''' posFile '''...']);
	dlmwrite(posFile,round(resampledPositions),'\t');
	disp(' ');
end

output = [targetTimestamps resampledPositions];

if shift ~= 0,
	output= [targetTimestamps+shift resampledPositions];
	if shift < 0,
		del = output(:,1)<0;
	else
		del = output(:,1)>output(end,1)-shift;
	end
	output(del,:) = [];
end

% ------------------------------------------------------------------------------------------

function cleanPositions = Clean(positions,maxGap,maxDistance)

if nargin < 2,
	maxGap = 20;
end

if nargin < 3,
	maxDistance = 30;
end

n = size(positions,1);

% Work on column pairs (each column pair corresponds to one LED)
for i = 1:size(positions,2)/2,

	% Interpolate large (instantaneous) jumps
	j = (i-1)*2+1;
	jump = abs(diff(positions(:,j)))>10|abs(diff(positions(:,j+1)))>10;
	good = find(~isnan(positions(:,j)) & ~([jump;0] | [0;jump]));
	if length(good) < 2,
		cleanPositions(:,[j j+1]) = nan(size(positions,1),2);
	else
		cleanPositions(:,[j j+1]) = round(interp1(good,positions(good,[j j+1]),1:n,'linear',nan));
	end

	% Find missing stretches
	d = [-isnan(positions(1,j)) ; diff(~isnan(positions(:,j)))];
	start = find(d<0);
	stop = find(d>0)-1;
	% (if last point is bad, final stretch should be discarded)
	if isnan(positions(end,j)),
		stop = [stop;n];
	end

	% Do not interpolate data that does not conform to (maxGap,maxDistance) constraints
	if length(start>0),
		i1 = Clip(start-1,1,n);
		i2 = Clip(stop+1,1,n);

		discard = find(stop-start>=maxGap ...
			| abs(positions(i1,j)-positions(i2,j)) > maxDistance ...
			| abs(positions(i1,j+1)-positions(i2,j+1)) > maxDistance);

		for k = discard(:),
			cleanPositions(start(k):stop(k),[j j+1]) = nan;
		end
	end

end

% ------------------------------------------------------------------------------------------
%   All functions below are copies or simplified versions of those found in FMAToolbox.
%   They are included here so that spots2pos does not require FMAToolbox.
% ------------------------------------------------------------------------------------------

function [X,Y,polygon] = UISelect

hold on;
X = [];
Y = [];
polygon = [];
last = false;

while ~last,
	% Get next mouse click
	[x,y,button] = ginput(1);
	switch button,
		case 1,
			% Left button adds this point
			X(end+1,1) = x;
			Y(end+1,1) = y;
		case 2,
			% Middle button closes the selection
			last = true;
			if ~isempty(X),
				X(end+1,1) = X(1);
				Y(end+1,1) = Y(1);
			end
		case 3,
			% Right button cancels last point
			if ~isempty(X),
				X(end) = [];
				Y(end) = [];
			end
	end
	% Update existing polygon
	if isempty(polygon),
		polygon = plot(X,Y);
	else
		warning('off','MATLAB:hg:line:XDataAndYDataLengthsMustBeEqual');
		set(polygon,'XData',X);
		set(polygon,'YData',Y);
		warning('on','MATLAB:hg:line:XDataAndYDataLengthsMustBeEqual');
	end
end

hold off;

% ------------------------------------------------------------------------------------------

function in = UIInPolygon(X,Y)

[Xp,Yp,p] = UISelect;
in = inpolygon(X,Y,Xp,Yp);
hold on;
pts = plot(X(in),Y(in),'+r');
hold off;
pause(0.4);
delete(p);
delete(pts);

% ------------------------------------------------------------------------------------------

function accumulated = Accumulate(variables,values,outputSize)

% Check input parameters
if isempty(variables),
	return;
end
if nargin < 2
	values = 1;
end
if nargin < 3
	outputSize = max(variables);
end
if size(variables,1) == 1, variables = variables'; end

% If 'values' is a scalar, make it a vector the same length as 'variables'
if length(values) == 1,
	values = repmat(values,size(variables,1),1);
end

% If 'variables' is a vector, make it a matrix
if size(variables,2) == 1,
	variables = [variables(:) ones(length(variables),1)];
	if length(outputSize) == 1,
		outputSize = [outputSize 1];
	end
end

% Drop NaN and Inf values
i = any(isnan(variables)|isinf(variables),2)|isnan(values)|isinf(values);
variables(i,:) = [];
values(i,:) = [];


% Accumulate
for i = 1:size(variables,2),
	subscript{i} = variables(:,i);
end
linearIndex = sub2ind(outputSize,subscript{:});
tmp = sparse(linearIndex,1,values,prod(outputSize),1);
accumulated = reshape(full(tmp),outputSize);

% ------------------------------------------------------------------------------------------

function events = LoadEvents(filename)

events.time = [];
events.description = [];
if ~exist(filename),
	error(['File ''' filename ''' not found.']);
end

file = fopen(filename,'r');
if file == -1,
	error(['Cannot read ' filename ' (insufficient access rights?).']);
end

while ~feof(file),
	time = fscanf(file,'%f',1);
	if isempty(time),
		if feof(file), break; end
		error(['Failed to read events from ' filename ' (possibly an empty file).']);
	end
	events.time(end+1,1) = time;
	line = fgetl(file);
	start = regexp(line,'[^\s]','once');
	events.description{end+1,1} = sscanf(line(start:end),'%c');
end
fclose(file);

% Convert to seconds
if ~isempty(events.time), events.time(:,1) = events.time(:,1) / 1000; end

% ------------------------------------------------------------------------------------------

function x = Clip(x,m,M)

x(x<m) = m;
x(x>M) = M;
