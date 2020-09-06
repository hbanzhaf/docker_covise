function [data] = crg_gen_csb2crg0(inc, u, v, c, s, b)
% CRG_GEN_CSB2CRG0 syntetical CRG-file generation.
%   [DATA] = CRG_GEN_CSB2CRG0(INC, U, V, C, S, B) generates a
%   syntetic CRG struct with a v-spaced grid with length u, increments inc,
%   curvature c, slope s, and banking b.
%
%   Inputs:
%   INC     increment (default: [0.1 0.1])
%           inc(1): u-increment
%           inc(2): v-increment
%   U       vector of u values
%           length(u) == 1: length of reference line
%           length(u) == 2: start/end value of reference line
%   V       vector of v values
%           length(v) == 1: defines half width of road
%           length(v) == 2: defines right/left edge of road
%           length(v) == nv: defines length cut positions
%   C       (nx2)-matrix of polynomial curvature values
%           c(i,1): u-length
%           c(i,2): polynomial
%   S       (nx2)-matrix of polynomial slope values
%           s(i,1): u-length
%           s(i,2): polynomial
%   B       (nx2)-matrix of polynomial banking values
%           b(i,1): u-length
%           b(i,2): polynomial

%
%   Outputs:
%   DATA    syntetic generated CRG file
%
%   Examples:
%   ulength = 350;
%
%   LC1  =   120;  C1s  =  inf;      C1e  =  inf;
%   LC2  =    50;  C2s  =  inf;      C2e  =  -50;
%   LC3  =   180;  C3s  =  -50;      C3e  =  -50;
%
%   LS1  =   120;  S1s  =  0;        S1e  =  0;
%   LS2  =    50;  S2s  =  0;        S2e  =  0;
%   LS3  =   180;  S3s  =  0;        S3e  =  0.03;
%
%   LB1  =   120;  B1s  =  0;        B1e  =  0;
%   LB2  =    50;  B2s  =  0;        B2e  =  0.02;
%   LB3  =   180;  B3s  =  0.02;     B3e  =  0.02;
%
% c = {  LC1    { 1/C1s ( 1/C1e - 1/C1s )/ LC1} ...
%     ;  LC2    { 1/C2s ( 1/C2e - 1/C2s )/ LC2} ...
%     ;  LC3    { 1/C3s ( 1/C3e - 1/C3s )/ LC3} ...
%     };
%
% s = { ...
%     ; LS1   { S1s  ( S1e  - S1s  )/LS1  }  ...
%     ; LS2   { S2s  ( S2e  - S2s  )/LS2  }  ...
%     ; LS3   { S3s  ( S3e  - S3s  )/LS3  }  ...
%     };
%
% b = { ...
%     ; LB1   { B1s  ( B1e  - B1s  )/LB1  }  ...
%     ; LB2   { B2s  ( B2e  - B2s  )/LB2  }  ...
%     ; LB3   { B3s  ( B3e  - B3s  )/LB3  }  ...
%     };
%
%   data = crg_gen_csb2crg0([1, 0.5], ulength, 2, c ,s ,b);
%
%   See also CRG_INTRO, CRG_TEST_GEN_CSB2CRG0.

%   Copyright 2005-2010 OpenCRG - VIRES Simulationstechnologie GmbH - HHelmich
%   Based on Dr. Klaus Mueller (DAIMLER AG) Matlab implementation to generate a synthetic road
%
%   Licensed under the Apache License, Version 2.0 (the "License");
%   you may not use this file except in compliance with the License.
%   You may obtain a copy of the License at
%
%       http://www.apache.org/licenses/LICENSE-2.0
%
%   Unless required by applicable law or agreed to in writing, software
%   distributed under the License is distributed on an "AS IS" BASIS,
%   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
%   See the License for the specific language governing permissions and
%   limitations under the License.
%
%   More Information on OpenCRG open file formats and tools can be found at
%
%       http://www.opencrg.org
%
%   $Id: crg_gen_csb2crg0 2010-03-25 10:20:00Z hhelmich $

%% check input parameter

if ~exist('c', 'var'), c = {}; end
if ~exist('b', 'var'), b = {}; end
if ~exist('s', 'var'), s = {}; end

if isempty(inc)     , inc = [0.01 0.01]; end    % default increment
if length(inc) == 1 , inc = [inc  0.01]; end    % default vinc

if length(u) < 1 || length(u) > 2
    error('CRG:checkError', 'check of u-spacing was not successful')
end

if length(v) < 1
    error('CRG:checkError', 'check of v-spacing was not successful')
end

%% evaluate/condense u
% ubeg
% uend
% uinc

uinc = inc(1);
switch length(u)
    case 1 % length of reference line
        ubeg = 0;
        uend = double(u(1));
        nu = ((uend) / uinc) + 1;
    case 2 % start/end value of u
        ubeg = double(u(1));
        uend = double(u(2));
        nu = ((uend-ubeg) / uinc) + 1;
    otherwise
        error('CRG:checkError', 'illegal size of u');
end

%% evaluate/condense v
% vmin
% vmax
% vinc (only for constant v spacing)

if ~issorted(v)
    error('CRG:checkError', 'vector DATA.v must be sorted')
end

switch length(v)
    case 1  % half width of road
        vmax = double(v(1));
        vmin = -vmax;
        vinc = inc(2);
        nv = ((2*vmax) / vinc)+1;
    case 2 % right/left edge of road
        vmin = double(v(1));
        vmax = double(v(2));
        vinc = inc(2);
        nv = ((abs(vmax)+abs(vmin)) / vinc) +1;
    otherwise % length cut positions
        vmin = double(v(1));
        vmax = double(v(end));
        nv = length(v);
end

%% build minimal CRG structure dout

data = struct;
data.head = struct;

data.head.ubeg = ubeg;
data.head.uend = uend;
data.head.uinc = uinc;
data.head.vmin = vmin;
data.head.vmax = vmax;
if exist('vinc', 'var')
    data.head.vinc = vinc;
end

data.u = u;
data.v = v;
data.z = zeros(fix(nu), fix(nv));

txtnum = 0;
txtnum = txtnum + 1; data.ct{txtnum} = 'CRG minimal artificial road and flat surface';

data = crg_single(data);

data = crg_check(data);
if ~isfield(data, 'ok')
    error('CRG:csb2crg0Error', 'check minimal CRG content was not completely successful')
end

%% Set Counters to initial values

err_cnt  = 0;
warn_cnt = 0;
uran = uend - ubeg;

%% simple check of curvature data

csum = 0;
for ii = 1:size(c,1);
    len = c{ii,1};
    if len < 0 || rem(len,uinc) >= data.opts.cinc || rem(uinc,data.opts.cinc) >= data.opts.ceps
        err_cnt = err_cnt + 1;
        warning('CRG:checkWarning', [num2str(ii) '. length = ' curvature num2str(len) ' is negative and/or mismatch with u - increment.  => fatal']);
    end
    if csum + len > uran
        err_cnt = err_cnt + 1;
        warning('CRG:checkWarning', [ num2str(ii) '. curvature length = ' num2str(len) ' exeeds range of u - coordinate. => fatal']);
    end
    csum = csum + len;
end
if ~isempty(c) && csum < uran
    warn_cnt = warn_cnt + 1;
    warning('CRG:checkWarning', 'A Straight line is added up to the end of u - coordinate range.');
    c = [ c; { max(0, uran-csum) {0} } ];    % if required add straight line up to the end
end

%% simple check of slope data

ssum = 0;
for ii = 1:size(s,1)
    len = s{ii,1};
    if len < 0 || rem(len,uinc) >= data.opts.cinc || rem(uinc,data.opts.cinc) >= data.opts.ceps
        err_cnt = err_cnt + 1;
        warning('CRG:checkWarning', [ num2str(ii) '. slope length = ' num2str(len) ' is negative and/or mismatch with u - increment. => fatal']);
    end
    if ssum + len > uran
        err_cnt = err_cnt + 1;
        warning('CRG:checkWarning', [ num2str(ii) '. slope length = ' num2str(len) ' exeeds range of u - coordinate. => fatal']);
    end
    ssum = ssum + len;
end
if ~isempty(s) && ssum < uran
    warn_cnt = warn_cnt + 1;
    warning('CRG:checkWarning', 'Last value of slope will be expanded up to the end of u-coordinate range.');
end

%% simple check of banking data

bsum = 0;
for ii = 1:size(b,1)
    len = b{ii,1};
    if len < 0 || rem(len,uinc) >= data.opts.cinc || rem(uinc,data.opts.cinc) >= data.opts.ceps
        err_cnt = err_cnt + 1;
        warning('CRG:checkWarning', [ num2str(ii) '. banking length = ' num2str(len) ' is negative and/or mismatch with u - increment. => fatal']);
    end
    if bsum + len > uran
        err_cnt = err_cnt + 1;
        warning('CRG:checkWarning', [ num2str(ii) '. banking length = ' num2str(len) ' exeeds range of u - coordinate. => fatal']);
    end
    bsum = bsum + len;
end
if ~isempty(b) && bsum < uran
    warn_cnt = warn_cnt + 1;
    warning('CRG:checkWarning', 'Last value of banking will be expanded up to the end of u - coordinate range.');
end

%% show error and warning count

if any([warn_cnt err_cnt])
    disp('Summary of check:')
    if warn_cnt > 0
        warning('CRG:checkWarning', ['Total Warnings: ' num2str(warn_cnt)]);
    end
    if err_cnt > 0
        error('CRG:checkError', ['Total   Errors: ' num2str(err_cnt)]);
    end
end

%% generate the road - curvature

if ~isempty(c)
    val    = crg_int_eval_poly(c);
    val    = angle(exp(1i*val));     % keep range between -pi to pi
    data.p = val(1:end-1);
    txtnum = txtnum + 1; data.ct{txtnum} = '... curvature added';

    data.head.pbeg = data.p(1);
    data.head.pend = data.p(end);
    data.head = rmfield(data.head, 'yend');
    data.head = rmfield(data.head, 'xend');
end

%% add slope

if ~isempty(s)
    val    = crg_eval_poly(s);
    data.s = val(1:end-1);
    txtnum = txtnum + 1; data.ct{txtnum} = '... slope added';

    data.head.sbeg = data.s(1);
    data.head.send = data.s(end);
    data.head = rmfield(data.head, 'zbeg');
    data.head = rmfield(data.head, 'zend');
end

%% add banking

if ~isempty(b)
    val    = crg_eval_poly(b);
    data.b = val;
    txtnum = txtnum + 1; data.ct{txtnum} = '... banking added';

    data.head.bbeg = data.b(1);
    data.head.bend = data.b(end);
end

%% check crg-struct

data = crg_single(data);

data = crg_check(data);
if ~isfield(data, 'ok')
    error('CRG:csb2crg0Error', 'check of DATA was not completely successful')
end

%% Nested function
function [out] = crg_int_eval_poly(upc)
% CRG_INT_EVAL_POLY(UPC) Integration of polynoms and evaluation along u
%
%   Inputs:
%   UPC     (nx2)-matrix of polynomial values
%           upc(i,1): u-length
%           upc(i,2): polynomial
%
%   Outputs:
%   OUT     vector of integrated polynoms and evaluation along u
%
%   See also CRG_INTRO

[n m] = size(upc);
if ~iscell(upc) || n < 1 || ~isequal(m, 2)
    error('CRG:checkError', 'upc must be a cell at least of size == (1,1)')
end

idx = 1;
out = zeros(1, nu, 'double');

for jj = 1:n
    ui  = 0:uinc:upc{jj,1};                    % u-space
    ide = idx+length(ui)-1;
    pc  = cell2mat(upc{jj,2}(:))';             % polynomial coefficients
    ipc = polyint(fliplr(pc),out(idx));        % integration of polynom
    out(idx:ide)= polyval(ipc, ui);            % evaluation of integrated polynom
    idx = ide;
end

end % crg_int_eval_poly

%% Nested function
function [out] = crg_eval_poly(upc)
% CRG_EVAL_POLY(UPC) Evaluation of polynoms along u
%
%   Inputs:
%   UPC     (nx2)-matrix of polynomial values
%           upc(i,1): u-length
%           upc(i,2): polynomial
%
%   Outputs:
%   OUT     vector of evaluation polynoms along u
%
%   See also CRG_INTRO

[n m] = size(upc);
if ~iscell(upc) || n < 1 || ~isequal(m, 2)
    error('CRG:checkError', 'upc must be a cell at least of size == (1,1)')
end

idx = 1;
out = zeros(1, nu, 'double');

for jj = 1:n
    ui  = 0:uinc:upc{jj,1};                    % u-space
    ide = idx+length(ui)-1;
    pc  = cell2mat(upc{jj,2}(:))';             % polynomial coefficients
    out(idx:ide)= polyval(fliplr(pc), ui);     % evaluation of polynom
    idx = ide;
end
out(idx:nu) = out(ide);                        % fill with last value up to the end

end % crg_eval_poly

end % function crg_gen_csb2crg0
