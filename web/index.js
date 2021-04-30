/*
   Copyright (c) 2019 Stefan Kremser
   This software is licensed under the MIT License. See the license file for details.
   Source: github.com/spacehuhn/WiFiDuck
 */

// ========== Global Variables ========== //

// ! List of files returned by "ls" command
var file_list = '';

// ! List of files returned by "ls" command
var profile_list = '';

// ! Variable to save interval for updating status continously
var status_interval = undefined;

// ! Unsaved content in the editor
var unsaved_changed = false;

// ! Unsaved profile changes
var unsaved_profile_changed = false;

// ! Flag if editor has loaded a file yet
var file_opened = false;

var profile_name_list = [];

// ========== Global Functions ========== //

function rgbToHex(r, g, b) {
  return '#' + ((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1);
}

function hexToRgb(hex) {
  // Expand shorthand form (e.g. "03F") to full form (e.g. "0033FF")
  var shorthandRegex = /^#?([a-f\d])([a-f\d])([a-f\d])$/i;
  hex = hex.replace(shorthandRegex, function (m, r, g, b) {
    return r + r + g + g + b + b;
  });

  var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
  return result
    ? {
        r: parseInt(result[1], 16),
        g: parseInt(result[2], 16),
        b: parseInt(result[3], 16),
      }
    : null;
}

var rgbToHexSimple = function (rgb) {
  var hex = Number(rgb).toString(16);
  if (hex.length < 2) {
    hex = `0${hex}`;
  }
  return hex;
};

var fullColorHex = function (r, g, b) {
  var red = rgbToHexSimple(r);
  var green = rgbToHexSimple(g);
  var blue = rgbToHexSimple(b);
  return `${red}${green}${blue}`;
};

function rgbStringToHex(rgb) {
  var [r, g, b] = rgb.split(',');
  return rgb ? `#${fullColorHex(r, g, b)}` : '#000';
}

function hexToRgbString(hex) {
  var rgb = hexToRgb(hex);
  return `${rgb.r},${rgb.g},${rgb.b}`;
}

// ===== Value Getters ===== //
function get_new_filename() {
  return E('newFile').value;
}

function get_new_profile_filename() {
  profile_name_list.sort();
  const [lastItem] = profile_name_list.slice(-1);
  console.log(lastItem);
  return `/profile${profile_name_list.length + 1}/config.json`;
}

function get_editor_filename() {
  return E('editorFile').value;
}

function set_editor_filename(filename) {
  return (E('editorFile').value = filename);
}

function get_editor_content() {
  var content = E('editor').value;

  if (!content.endsWith('\n')) content = content + '\n';

  return content;
}

// ! Update status until it's no longer "running"
function check_status() {
  if (current_status.includes('running') || current_status.includes('saving')) ws_update_status();
  else stop_status_interval();
}

// ! Start interval that checks and updates the status continously
function start_status_interval() {
  if (status_interval) return; // !< Only continue if status_interval not set

  ws_update_status(); // !< Get current status
  status_interval = setInterval(check_status, 500); // !< Start interval
}

// ! Stop interval that checks and updates the status continously
function stop_status_interval() {
  if (!status_interval) return; // !< Only continue if status_interval was set

  // ! Stop interval and unset variable
  clearInterval(status_interval);
  status_interval = undefined;
}

// ! Append string to script content
function append(str) {
  E('editor').value += str;
}

// ! Updates file list and memory usage
function update_profile_list() {
  ws_send('mem', function (msg) {
    var lines = msg.split(/\n/);
    var byte = lines[0].split(' ')[0];
    var used = lines[1].split(' ')[0];
    var free = lines[2].split(' ')[0];

    var percent = Math.floor(byte / 100);
    var freepercent = Math.floor(free / percent);

    E('freeMemory').innerHTML = used + ' byte used (' + freepercent + '% free)';

    profile_list = '';
    profile_name_list = [];

    ws_send('ls', function (csv) {
      profile_list += csv;

      var lines = profile_list.split(/\n/);
      var tableHTML = '<thead>\n';

      tableHTML += '<tr>\n';
      tableHTML += '<th>File</th>\n';
      tableHTML += '<th>Byte</th>\n';
      tableHTML += '<th>Actions</th>\n';
      tableHTML += '</tr>\n';
      tableHTML += '</thead>\n';
      tableHTML += '<tbody>\n';

      for (var i = 0; i < lines.length; i++) {
        var data = lines[i].split(' ');
        var fileName = data[0];
        var fileSize = data[1];
        var profileNameParts = fileName.split('/');
        var profileName = profileNameParts[1];

        if (fileName.includes('config.json')) {
          /*if (i == 0 && !file_opened) {
            read(fileName);
          }*/
          tableHTML += '<tr>\n';
          tableHTML += '<td>' + profileName + '</td>\n';
          tableHTML += '<td>' + fileSize + '</td>\n';
          tableHTML += '<td>\n';
          tableHTML += '<button class="primary" onclick="selectProfile(\'' + profileName + '\')">edit</button>\n';
          //tableHTML += '<button class="warn" onclick="selectProfile(\'' + profileName + '\')">select</button>\n';
          tableHTML += '</tr>\n';
          profile_name_list.push(profileName);
        }
      }
      tableHTML += '</tbody>\n';

      E('profileTable').innerHTML = tableHTML;
    });
  });
}

var currentProfileName = '';
var currentProfile = '';
var currentKey = '';
var currentKeyName = '';
var loadProfile = false;
var loadKey = false;
var currentProfileObj = undefined;

function selectProfile(profileName) {
  console.log(profileName);
  currentProfileName = profileName;
  E('profileTitle').innerText = profileName;
  var fileName = `/${profileName}/config.json`;
  loadProfile = true;
  loadKey = false;
  updateKeyForm();
  currentProfile = '';
  read(fileName);
}

function updateProfileForm() {
  if (loadProfile) {
    currentProfileObj = JSON.parse(currentProfile);
    var profile = currentProfileObj;
    console.log(profile);
    E('profileSection').style.display = 'block';

    E('profileName').value = profile.name;
    E('profileSize').value = profile.size;
    E('profileUpColor').value = rgbStringToHex(profile.bg_color);
    E('profileDownColor').value = rgbStringToHex(profile.down_color);
    E('profileUpSpeed').value = profile.up_speed ? profile.up_speed : 10;
    E('profileDownSpeed').value = profile.down_speed ? profile.down_speed : 0;

    var keys = [
      { id: 'key1', profile: profile.key1 },
      { id: 'key2', profile: profile.key2 },
      { id: 'key3', profile: profile.key3 },
      { id: 'key4', profile: profile.key4 },
      { id: 'key5', profile: profile.key5 },
      { id: 'key6', profile: profile.key6 },
      { id: 'key7', profile: profile.key7 },
      { id: 'key8', profile: profile.key8 },
      { id: 'key9', profile: profile.key9 },
      { id: 'key10', profile: profile.key10 },
      { id: 'key11', profile: profile.key11 },
      { id: 'enc', profile: profile.enc },
      { id: 'enc_up', profile: profile.enc_up },
      { id: 'enc_down', profile: profile.enc_down },
    ];

    for (var i = 0; i < keys.length; i++) {
      var key = keys[i];
      if (key.id.startsWith('enc')) {
        if (key.profile) {
          if (key.id === 'enc') E(`${key.id}`).innerText = key.profile.name;
          var key_up = key.profile.bg_color
            ? `rgba(${key.profile.bg_color},1)`
            : profile.bg_color
            ? `rgba(${profile.bg_color},1)`
            : 'rgba(34, 37, 42, 0.5)';
          var key_down = key.profile.down_color
            ? `rgba(${key.profile.down_color},1)`
            : profile.down_color
            ? `rgba(${profile.down_color},1)`
            : 'rgba(34, 37, 42, 0.5)';
          E(`${key.id}`).style = `--color-up: ${key_up}; --color-down: ${key_down};`;
        } else {
          if (key.id === 'enc') E(`${key.id}`).innerText = '';
          E(`${key.id}`).style = `--color-up: rgba(34, 37, 42, 0.5); --color-down: rgba(34, 37, 42, 0.5);`;
        }
      } else {
        if (key.profile) {
          C(`${key.id}`).innerText = key.profile.name;
          var key_up = key.profile.bg_color
            ? `rgba(${key.profile.bg_color},1)`
            : profile.bg_color
            ? `rgba(${profile.bg_color},1)`
            : 'rgba(34, 37, 42, 0.5)';
          var key_down = key.profile.down_color
            ? `rgba(${key.profile.down_color},1)`
            : profile.down_color
            ? `rgba(${profile.down_color},1)`
            : 'rgba(34, 37, 42, 0.5)';
          E(`${key.id}`).style = `--color-up: ${key_up}; --color-down: ${key_down};`;
        } else {
          C(`${key.id}`).innerText = '';
          E(`${key.id}`).style = `--color-up: rgba(34, 37, 42, 0.5); --color-down: rgba(34, 37, 42, 0.5);`;
        }
      }
    }
  } else {
    console.log('No profile selected!');
  }
  loadProfile = false;
}

function selectKey(key) {
  console.log(key);
  currentKeyName = key;
  E('keyTitle').innerText = key;
  var fileName = `/${currentProfileName}/${key}.txt`;
  loadKey = true;
  loadProfile = false;
  currentKey = '';
  read(fileName);
}

function updateKeyForm() {
  if (loadKey) {
    E('keySection').style.display = 'block';

    var name = currentKeyName;
    /*
    if (currentKeyName.startsWith('enc')) {
      name = 'encoder';
    }
    */

    var key = currentProfileObj[name];
    if (!key) {
      currentProfileObj[name] = { name: '' };
      key = currentProfileObj[name];
    }
    console.log(key);

    E('keyName').value = key.name;
    E('keySize').value = key.size;
    E('keyUpColor').value = rgbStringToHex(
      key.bg_color ? key.bg_color : currentProfileObj.bg_color ? currentProfileObj.bg_color : '0,0,0'
    );
    E('keyDownColor').value = rgbStringToHex(
      key.down_color ? key.down_color : currentProfileObj.down_color ? currentProfileObj.down_color : '0,0,0'
    );
    E('keyUpSpeed').value = key.up_speed ? key.up_speed : 10;
    E('keyDownSpeed').value = key.down_speed ? key.down_speed : 0;
  } else {
    console.log('No key selected!');
    E('keySection').style.display = 'none';
  }
  loadKey = false;
}

// ! Format SPIFFS
function format() {
  if (confirm('Format SPIFFS? This will delete all scripts!')) {
    ws_send('format', log_ws);
    alert('Formatting will take a minute.\nYou have to reconnect afterwards.');
  }
}

// ! Run script
function run(fileName) {
  ws_send('run "' + fixFileName(fileName) + '"', log_ws);
  start_status_interval();
}

// ! Stop running specific script
function stop(fileName) {
  ws_send('stop "' + fixFileName(fileName) + '"', log_ws, true);
}

// ! Stop running all scripts
function stopAll() {
  ws_send('stop', log_ws, true);
}

// ! Recursive read from stream
function read_stream() {
  ws_send('read', function (content) {
    if (content != '> END') {
      E('editor').value += content;
      if (loadProfile) {
        currentProfile += content;
      } else if (loadKey) {
        currentKey += content;
      }
      read_stream();
      status('reading...');
    } else {
      ws_send('close', log_ws);
      ws_update_status();
      if (loadProfile) {
        updateProfileForm();
      } else if (loadKey) {
        updateKeyForm();
      }
    }
  });
}

// ! Open stream to a file
function read(fileName) {
  stop(fileName);

  fileName = fixFileName(fileName);

  set_editor_filename(fileName);
  E('editor').value = '';

  ws_send('stream "' + fileName + '"', log_ws);

  read_stream(); // !< Read file contents (recursively)

  file_opened = true;
}

// ! Create a new file
function create(fileName) {
  stop(fileName);

  fileName = fixFileName(fileName);

  if (file_list.includes(fileName + ' ')) {
    read(fileName);
  } else {
    set_editor_filename(fileName);
    E('editor').value = '';

    ws_send('create "' + fileName + '"', log_ws);
    update_profile_list();
  }
}

function createProfile() {
  var fileName = get_new_profile_filename();
  stop(fileName);

  fileName = fixFileName(fileName);

  if (file_list.includes(fileName + ' ')) {
    read(fileName);
  } else {
    set_editor_filename(fileName);
    E('editor').value = '';

    ws_send('create "' + fileName + '"', log_ws);
    var newProfileObj = {
      name: `pro${profile_name_list.length + 1}`,
      bg_color: '0,0,0',
      down_color: '0,100,255',
      size: 2,
    };

    var content = JSON.stringify(newProfileObj);
    write(fileName, content);
    update_profile_list();
  }
}

// ! Delete a file
function remove(fileName) {
  stop(fileName);
  ws_send('remove "' + fixFileName(fileName) + '"', log_ws);
  update_profile_list();
  unsaved_changed = true;
}

function autorun(fileName) {
  ws_send('set autorun "' + fixFileName(fileName) + '"', log_ws);
}

// ! Write content to file
function write(fileName, content) {
  stop(fileName);

  fileName = fixFileName(fileName);

  ws_send('remove "/temporary_script"', log_ws);
  ws_send('create "/temporary_script"', log_ws);

  ws_send('stream "/temporary_script"', log_ws);

  var ws_send_log = function (msg) {
    status('saving...');
    log_ws(msg);
  };

  var pktsize = 4096;

  for (var i = 0; i < Math.ceil(content.length / pktsize); i++) {
    var begin = i * pktsize;
    var end = begin + pktsize;
    if (end > content.length) end = content.length;

    ws_send_raw(content.substring(begin, end), ws_send_log);
  }

  ws_send('close', log_ws);

  ws_send('remove "' + fileName + '"', log_ws);
  ws_send('rename "/temporary_script" "' + fileName + '"', log_ws);

  ws_update_status();
}

// ! Save file that is currently open in the editor
function save() {
  write(get_editor_filename(), get_editor_content());
  unsaved_changed = false;
  E('editorinfo').innerHTML = 'saved';
  update_profile_list();
}

// ! Save file that is currently open in the editor
function saveProfile() {
  currentProfileObj.name = E('profileName').value;
  currentProfileObj.size = parseInt(E('profileSize').value);
  currentProfileObj.bg_color = hexToRgbString(E('profileUpColor').value);
  currentProfileObj.down_color = hexToRgbString(E('profileDownColor').value);
  currentProfileObj.up_speed = parseInt(E('profileUpSpeed').value);
  currentProfileObj.down_speed = parseInt(E('profileDownSpeed').value);

  var name = currentKeyName;
  /*
  if (currentKeyName.startsWith('enc')) {
    name = 'encoder';
  }
  */

  if (E('keyName').value) {
    currentProfileObj[name].name = E('keyName').value;
    currentProfileObj[name].size = parseInt(E('keySize').value);
    currentProfileObj[name].bg_color =
      E('keyUpColor').value !== E('profileUpColor').value ? hexToRgbString(E('keyUpColor').value) : undefined;
    currentProfileObj[name].down_color =
      E('keyDownColor').value !== E('profileDownColor').value ? hexToRgbString(E('keyDownColor').value) : undefined;
    currentProfileObj[name].up_speed =
      E('keyUpSpeed').value !== E('profileUpSpeed').value ? parseInt(E('keyUpSpeed').value) : undefined;
    currentProfileObj[name].down_speed =
      E('keyDownSpeed').value !== E('profileDownSpeed').value ? parseInt(E('keyDownSpeed').value) : undefined;
  } else {
    delete currentProfileObj[name];
  }

  currentProfile = JSON.stringify(currentProfileObj);
  var fileName = `/${currentProfileName}/config.json`;

  console.log(currentProfileObj);

  write(fileName, currentProfile);
  unsaved_profile_changed = false;
  E('editorinfo').innerHTML = 'saved';
  loadProfile = true;
  updateProfileForm();
  if (loadKey && currentKeyName) selectKey(currentKeyName);
  //updateProfileForm();

  //update_profile_list();
}

// ! Function that is called once the websocket connection was established
function ws_connected() {
  update_profile_list();
}

// ========== Startup ========== //
window.addEventListener(
  'load',
  function () {
    E('reconnect').onclick = ws_init;
    //E("scriptsReload").onclick = update_file_list;
    E('reloadProfiles').onCLick = update_profile_list;
    E('format').onclick = format;
    E('stop').onclick = stopAll;

    E('editorReload').onclick = function () {
      read(get_editor_filename());
    };

    E('editorSave').onclick = save;

    E('profileSave').onclick = saveProfile;

    E('editorDelete').onclick = function () {
      if (confirm('Delete ' + get_editor_filename() + '?')) {
        remove(get_editor_filename());
      }
    };

    E('editorDownload').onclick = function () {
      download_txt(get_editor_filename(), get_editor_content());
    };

    E('editorStop').onclick = function () {
      stop(get_editor_filename());
    };

    E('editorRun').onclick = function () {
      if (unsaved_changed) {
        save();
      }

      run(get_editor_filename());
    };

    E('editor').onkeyup = function () {
      unsaved_changed = true;
      E('editorinfo').innerHTML = 'unsaved changes';
    };

    E('editorAutorun').onclick = function () {
      if (confirm('Run this script automatically on startup?\nYou can disable it in the settings.'))
        autorun(get_editor_filename());
    };

    E('profileName').oninput = function () {
      unsaved_profile_changed = true;
    };

    E('profileSize').oninput = function () {
      unsaved_profile_changed = true;
    };

    E('profileUpColor').oninput = function () {
      unsaved_profile_changed = true;
    };

    E('profileDownColor').oninput = function () {
      unsaved_profile_changed = true;
    };

    E('profileUpSpeed').oninput = function () {
      unsaved_profile_changed = true;
    };

    E('profileDownSpeed').oninput = function () {
      unsaved_profile_changed = true;
    };

    E('keyName').oninput = function () {
      unsaved_profile_changed = true;
    };

    E('keySize').oninput = function () {
      unsaved_profile_changed = true;
    };

    E('keyUpColor').oninput = function () {
      unsaved_profile_changed = true;
    };

    E('keyDownColor').oninput = function () {
      unsaved_profile_changed = true;
    };

    E('keyUpSpeed').oninput = function () {
      unsaved_profile_changed = true;
    };

    E('keyDownSpeed').oninput = function () {
      unsaved_profile_changed = true;
    };

    // ! Make all <code>s append to the editor when clicked
    var codes = document.querySelectorAll('code');
    for (var i = 0; i < codes.length; i++) {
      codes[i].addEventListener('click', function () {
        append(this.innerHTML + ' \n');
      });
    }

    ws_init();
  },
  false
);
