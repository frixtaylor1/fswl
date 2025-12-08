local file = io.open("stress_test/test.json", "rb")
if file == nil then
    io.stderr:write("Error: No se pudo abrir test.json\n")
    os.exit(1)
end
local json_body = file:read("*a")
file:close()

request = function()
   return wrk.format("POST", "/hello", {
       ["Content-Type"] = "application/json"
   }, json_body)
end