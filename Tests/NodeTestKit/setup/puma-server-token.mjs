#!/usr/bin/env node
/**
	Setup step of the Puma service, of the kit: gives the server under test an access token for Puma.

	The server reads the users and groups of Puma with this token when a read has no caller, such as the recipients
	of a mailed report. The step runs on the fresh Puma database before the server starts:
	- it creates the superuser with the credentials of the collection environment, so folder 00 finds it,
	- a role that may only view users, groups and roles,
	- a user with that role,
	- and a personal access token of that user, scoped to those permissions and without an expiry date.
	It writes the token as the environment variable --env-name, which the test composition of the product reads with
	ImtTestPck/EnvironmentIdParam.
	The collection environment holds productId, suMail and suPassword.
	Usage: node puma-server-token.mjs --http <Puma URL> --env <environment.json> --out <file>
		[--user <login of the server user>] [--env-name <variable of the token>]
	In suite.json: "setup": { "name": "puma-server-token", "kit": "puma-server-token.mjs", "arguments": { "user": "..." } }
*/

import fs from 'node:fs';
import { parseArgs } from 'node:util';


const { values: options } = parseArgs({
	options: {
		'http': { type: 'string' },
		'env': { type: 'string' },
		'out': { type: 'string' },
		'user': { type: 'string', default: 'test_server' },
		'env-name': { type: 'string', default: 'TEST_PUMA_ACCESS_TOKEN' },
	},
});

const environment = JSON.parse(fs.readFileSync(options.env, 'utf8'));
const variables = Object.fromEntries(environment.values.map((value) => [value.key, value.value]));
const graphqlUrl = `${options.http}/Puma/graphql`;
const productId = variables.productId;
const viewPermissions = ['/Administration/Users/View', '/Administration/Groups/View', '/Administration/Roles/View'];


function Log(message)
{
	console.log(`[puma-server-token] ${message}`);
}


/**
	The operation is named by the caller and not cut out of the query: the query holds the passwords, and a message made
	from it would leak them into the log.
*/
async function Request(operation, query, token)
{
	const headers = { 'Content-Type': 'application/json', 'productid': productId };
	if (token !== undefined){
		headers['x-authentication-token'] = token;
	}

	const response = await fetch(graphqlUrl, { method: 'POST', headers, body: JSON.stringify({ query }) });
	const json = await response.json();
	if (json.errors !== undefined){
		throw new Error(`${operation} failed: ${JSON.stringify(json.errors)}`);
	}

	return json.data;
}


async function Main()
{
	const created = await Request('CreateSuperuser', `mutation { CreateSuperuser(input: { name: "Super User", mail: ${JSON.stringify(variables.suMail)}, password: ${JSON.stringify(variables.suPassword)} }) { success message } }`);
	if (!created.CreateSuperuser.success){
		throw new Error(`CreateSuperuser failed: ${created.CreateSuperuser.message}`);
	}

	const login = await Request('Authorization', `query { Authorization(input: { login: "su", password: ${JSON.stringify(variables.suPassword)}, productId: "${productId}" }) { token } }`);
	const superuserToken = login.Authorization.token;

	const permissions = viewPermissions.map((permission) => `{ id: "${permission}", direct: true }`).join(', ');
	const role = await Request('RoleAdd', `mutation { RoleAdd(input: { id: "", typeId: "Role", productId: "${productId}", name: "Server reader", description: "",
		item: { id: "", roleId: "server_reader", productId: "${productId}", name: "Server reader", description: "Reads the users, groups and roles", permissions: [${permissions}] } }) { id } }`, superuserToken);

	const user = await Request('UserAdd', `mutation { UserAdd(input: { id: "", typeId: "User", productId: "${productId}", name: "${options.user}", description: "",
		item: { id: "", productId: "${productId}", username: "${options.user}", name: "${options.user}", password: ${JSON.stringify(`${variables.suPassword}-server`)}, email: "${options.user}@${productId.toLowerCase()}.test", enabled: true,
		roles: [{ id: "${role.RoleAdd.id}", direct: true }] } }) { id } }`, superuserToken);

	const scopes = viewPermissions.map((permission) => `"${permission}"`).join(', ');
	const token = await Request('CreateToken', `mutation { CreateToken(input: { userId: "${user.UserAdd.id}", productId: "${productId}", name: "${options.user}",
		description: "Reads the users, groups and roles", scopes: [${scopes}] }) { token success message } }`, superuserToken);
	if (!token.CreateToken.success || !token.CreateToken.token){
		throw new Error(`CreateToken failed: ${token.CreateToken.message}`);
	}

	fs.writeFileSync(options.out, JSON.stringify({ environment: { [options['env-name']]: token.CreateToken.token } }));
	Log('superuser, reader role, server user and its access token are created');
}


Main().catch((error) => {
	Log(error.message);
	process.exit(1);
});
