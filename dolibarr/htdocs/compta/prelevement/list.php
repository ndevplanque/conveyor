<?php
/* Copyright (C) 2005		Rodolphe Quiedeville		<rodolphe@quiedeville.org>
 * Copyright (C) 2005-2016	Laurent Destailleur			<eldy@users.sourceforge.net>
 * Copyright (C) 2005-2009	Regis Houssin				<regis.houssin@inodbox.com>
 * Copyright (C) 2010-2018	Juanjo Menent				<jmenent@2byte.es>
 * Copyright (C) 2022-2024	Alexandre Spangaro			<alexandre@inovea-conseil.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

/**
 *      \file       htdocs/compta/prelevement/list.php
 *      \ingroup    prelevement
 *      \brief      Page to list direct debit orders or credit transfers orders
 */

// Load Dolibarr environment
require '../../main.inc.php';
require_once DOL_DOCUMENT_ROOT.'/compta/prelevement/class/bonprelevement.class.php';
require_once DOL_DOCUMENT_ROOT.'/compta/prelevement/class/ligneprelevement.class.php';
require_once DOL_DOCUMENT_ROOT.'/compta/bank/class/account.class.php';

// Load translation files required by the page
$langs->loadLangs(array('banks', 'withdrawals', 'companies', 'categories'));

$action     = GETPOST('action', 'aZ09') ? GETPOST('action', 'aZ09') : 'view'; // The action 'add', 'create', 'edit', 'update', 'view', ...
$massaction = GETPOST('massaction', 'alpha'); // The bulk action (combo box choice into lists)
$show_files = GETPOSTINT('show_files'); // Show files area generated by bulk actions ?
$confirm    = GETPOST('confirm', 'alpha'); // Result of a confirmation
$cancel     = GETPOST('cancel', 'alpha'); // We click on a Cancel button
$toselect   = GETPOST('toselect', 'array'); // Array of ids of elements selected into a list
$contextpage = GETPOST('contextpage', 'aZ') ? GETPOST('contextpage', 'aZ') : 'directdebitcredittransferlinelist'; // To manage different context of search
$backtopage = GETPOST('backtopage', 'alpha'); // Go back to a dedicated page
$optioncss  = GETPOST('optioncss', 'aZ'); // Option for the css output (always '' except when 'print')
$mode       = GETPOST('mode', 'aZ'); // The output mode ('list', 'kanban', 'hierarchy', 'calendar', ...)

$type = GETPOST('type', 'aZ09');

// Load variable for pagination
$limit = GETPOSTINT('limit') ? GETPOSTINT('limit') : $conf->liste_limit;
$sortfield = GETPOST('sortfield', 'aZ09comma');
$sortorder = GETPOST('sortorder', 'aZ09comma');
$page = GETPOSTISSET('pageplusone') ? (GETPOSTINT('pageplusone') - 1) : GETPOSTINT("page");
if (empty($page) || $page < 0 || GETPOST('button_search', 'alpha') || GETPOST('button_removefilter', 'alpha')) {
	// If $page is not defined, or '' or -1 or if we click on clear filters
	$page = 0;
}
$offset = $limit * $page;
$pageprev = $page - 1;
$pagenext = $page + 1;
if (!$sortorder) {
	$sortorder = "DESC";
}
if (!$sortfield) {
	$sortfield = ($type == 'bank-transfer' ? "datec" : "p.datec");
}

$search_line = GETPOST('search_line', 'alpha');
$search_bon = GETPOST('search_bon', 'alpha');
$search_code = GETPOST('search_code', 'alpha');
$search_company = GETPOST('search_company', 'alpha');
$statut = GETPOSTINT('statut');

$bon = new BonPrelevement($db);
$line = new LignePrelevement($db);
$company = new Societe($db);
$userstatic = new User($db);

$hookmanager->initHooks(array('withdrawalsreceiptslineslist'));

// Security check
$socid = GETPOSTINT('socid');
if ($user->socid) {
	$socid = $user->socid;
}
if ($type == 'bank-transfer') {
	$result = restrictedArea($user, 'paymentbybanktransfer', '', '', '');
} else {
	$result = restrictedArea($user, 'prelevement', '', '', 'bons');
}


/*
 * Actions
 */

if (GETPOST('button_removefilter_x', 'alpha') || GETPOST('button_removefilter.x', 'alpha') || GETPOST('button_removefilter', 'alpha')) { // All tests are required to be compatible with all browsers
	$search_line = "";
	$search_bon = "";
	$search_code = "";
	$search_company = "";
	$statut = "";
}


/*
 *  View
 */

$form = new Form($db);

$title = $langs->trans("WithdrawalsLines");
if ($type == 'bank-transfer') {
	$title = $langs->trans("CreditTransferLines");
}
$help_url = '';

$sql  = "SELECT p.rowid, p.ref, p.statut as status, p.datec";
$sql .= " , f.rowid as facid, f.ref as invoiceref, f.total_ttc";
$sql .= " , s.rowid as socid, s.nom as name, s.code_client, s.code_fournisseur, s.email";
$sql .= " , pl.amount, pl.statut as statut_ligne, pl.rowid as rowid_ligne";

$sqlfields = $sql; // $sql fields to remove for count total

$sql .= " FROM ".MAIN_DB_PREFIX."prelevement_bons as p";
$sql .= " , ".MAIN_DB_PREFIX."prelevement_lignes as pl";
$sql .= " , ".MAIN_DB_PREFIX."prelevement as pf";
if ($type == 'bank-transfer') {
	$sql .= " , ".MAIN_DB_PREFIX."facture_fourn as f";
} else {
	$sql .= " , ".MAIN_DB_PREFIX."facture as f";
}
$sql .= " , ".MAIN_DB_PREFIX."societe as s";
$sql .= " WHERE pl.fk_prelevement_bons = p.rowid";
$sql .= " AND pf.fk_prelevement_lignes = pl.rowid";
if ($type == 'bank-transfer') {
	$sql .= " AND pf.fk_facture_fourn = f.rowid";
} else {
	$sql .= " AND pf.fk_facture = f.rowid";
}
$sql .= " AND f.fk_soc = s.rowid";
$sql .= " AND f.entity IN (".getEntity('invoice').")";
if ($socid) {
	$sql .= " AND s.rowid = ".((int) $socid);
}
if ($search_bon) {
	$sql .= " AND pl.rowid = '".$db->escape($search_bon)."'";
}
if ($search_line) {
	$sql .= natural_search("p.ref", $search_line);
}
if ($type == 'bank-transfer') {
	if ($search_code) {
		$sql .= natural_search("s.code_fournisseur", $search_code);
	}
} else {
	if ($search_code) {
		$sql .= natural_search("s.code_client", $search_code);
	}
}
if ($search_company) {
	$sql .= natural_search("s.nom", $search_company);
}
//get salary invoices
if ($type == 'bank-transfer') {
	$sql .= " UNION";

	$sql .= " SELECT p.rowid, p.ref, p.statut as status, p.datec";
	$sql .= ", sl.rowid as facid, sl.ref as invoiceref, sl.amount";
	$sql .= ", u.rowid as socid, CONCAT(u.firstname, ' ', u.lastname) as name, u.ref_employee as code_client, NULL as code_fournisseur, u.email";
	$sql .= ", pl.amount, pl.statut as statut_ligne, pl.rowid as rowid_ligne";

	$sql .= " FROM ".MAIN_DB_PREFIX."prelevement_bons as p";
	$sql .= " , ".MAIN_DB_PREFIX."prelevement_lignes as pl";
	$sql .= " , ".MAIN_DB_PREFIX."prelevement as pf";
	$sql .= " , ".MAIN_DB_PREFIX."salary as sl";
	$sql .= " , ".MAIN_DB_PREFIX."user as u";

	$sql .= " WHERE pl.fk_prelevement_bons = p.rowid";
	$sql .= " AND pf.fk_prelevement_lignes = pl.rowid";
	$sql .= " AND pf.fk_salary = sl.rowid";
	$sql .= " AND sl.fk_user = u.rowid";
	$sql .= " AND sl.entity IN (".getEntity('invoice').")";
	if ($socid) {
		$sql .= " AND s.rowid = ".((int) $socid);
	}
	if ($search_bon) {
		$sql .= " AND pl.rowid = '".$db->escape($search_bon)."'";
	}
	if ($search_line) {
		$sql .= natural_search("p.ref", $search_line);
	}
	if ($type == 'bank-transfer') {
		if ($search_code) {
			$sql .= natural_search("NULL", $search_code);
		}
	} else {
		if ($search_code) {
			$sql .= natural_search("s.code_client", $search_code);
		}
	}
	if ($search_company) {
		$sql .= natural_search(array("u.firstname","u.lastname"), $search_company);
	}
}
// Count total nb of records
$nbtotalofrecords = '';
if (!getDolGlobalInt('MAIN_DISABLE_FULL_SCANLIST')) {
	/* The fast and low memory method to get and count full list converts the sql into a sql count */
	if ($type == 'bank-transfer') {
		$sqlforcount = "SELECT COUNT(*) as nbtotalofrecords FROM (" . $sql . ") AS combined_results";
	} else {
		$sqlforcount = preg_replace('/^'.preg_quote($sqlfields, '/').'/', 'SELECT COUNT(*) as nbtotalofrecords', $sql);
		$sqlforcount = preg_replace('/GROUP BY .*$/', '', $sqlforcount);
	}

	$resql = $db->query($sqlforcount);
	if ($resql) {
		$objforcount = $db->fetch_object($resql);
		$nbtotalofrecords = $objforcount->nbtotalofrecords;
	} else {
		dol_print_error($db);
	}

	if (($page * $limit) > $nbtotalofrecords) {	// if total resultset is smaller than the paging size (filtering), goto and load page 0
		$page = 0;
		$offset = 0;
	}
	$db->free($resql);
}

// Complete request and execute it with limit
$sql .= $db->order($sortfield, $sortorder);
if ($limit) {
	$sql .= $db->plimit($limit + 1, $offset);
}

$resql = $db->query($sql);
if (!$resql) {
	dol_print_error($db);
	exit;
}

$num = $db->num_rows($resql);

// Output page
// --------------------------------------------------------------------

llxHeader('', $title, $help_url, 0, 0, '', '', '', 'bodyforlist');

$arrayofselected = is_array($toselect) ? $toselect : array();

$param = '';
$param .= "&statut=".urlencode($statut);
$param .= "&search_bon=".urlencode($search_bon);
if ($type == 'bank-transfer') {
	$param .= '&type=bank-transfer';
}
if (!empty($mode)) {
	$param .= '&mode='.urlencode($mode);
}
if (!empty($contextpage) && $contextpage != $_SERVER["PHP_SELF"]) {
	$param .= '&contextpage='.urlencode($contextpage);
}
if ($limit > 0 && $limit != $conf->liste_limit) {
	$param .= '&limit='.((int) $limit);
}
if ($optioncss != '') {
	$param .= '&optioncss='.urlencode($optioncss);
}

$arrayofmassactions = array(
	//'presend'=>img_picto('', 'email', 'class="pictofixedwidth"').$langs->trans("SendByMail"),
	//'builddoc'=>img_picto('', 'pdf', 'class="pictofixedwidth"').$langs->trans("PDFMerge"),
);
$massactionbutton = $form->selectMassAction('', $arrayofmassactions);

print '<form method="POST" id="searchFormList" action="'.$_SERVER["PHP_SELF"].'">'."\n";
print '<input type="hidden" name="token" value="'.newToken().'">';
if ($optioncss != '') {
	print '<input type="hidden" name="optioncss" value="'.$optioncss.'">';
}
print '<input type="hidden" name="formfilteraction" id="formfilteraction" value="list">';
print '<input type="hidden" name="action" value="list">';
print '<input type="hidden" name="sortfield" value="'.$sortfield.'">';
print '<input type="hidden" name="sortorder" value="'.$sortorder.'">';
print '<input type="hidden" name="page" value="'.$page.'">';
print '<input type="hidden" name="contextpage" value="'.$contextpage.'">';
print '<input type="hidden" name="page_y" value="">';
print '<input type="hidden" name="mode" value="'.$mode.'">';

if ($type != '') {
	print '<input type="hidden" name="type" value="'.$type.'">';
}

$newcardbutton = '';
$newcardbutton .= dolGetButtonTitle($langs->trans('ViewList'), '', 'fa fa-bars imgforviewmode', $_SERVER["PHP_SELF"].'?mode=common'.preg_replace('/(&|\?)*mode=[^&]+/', '', $param), '', ((empty($mode) || $mode == 'common') ? 2 : 1), array('morecss'=>'reposition'));
$newcardbutton .= dolGetButtonTitle($langs->trans('ViewKanban'), '', 'fa fa-th-list imgforviewmode', $_SERVER["PHP_SELF"].'?mode=kanban'.preg_replace('/(&|\?)*mode=[^&]+/', '', $param), '', ($mode == 'kanban' ? 2 : 1), array('morecss'=>'reposition'));

print_barre_liste($title, $page, $_SERVER["PHP_SELF"], $param, $sortfield, $sortorder, $massactionbutton, $num, $nbtotalofrecords, 'generic', 0, $newcardbutton, '', $limit, 0, 0, 1);

$moreforfilter = '';
/*$moreforfilter.='<div class="divsearchfield">';
 $moreforfilter.= $langs->trans('MyFilter') . ': <input type="text" name="search_myfield" value="'.dol_escape_htmltag($search_myfield).'">';
 $moreforfilter.= '</div>';*/

$parameters = array();
$reshook = $hookmanager->executeHooks('printFieldPreListTitle', $parameters, $object, $action); // Note that $action and $object may have been modified by hook
if (empty($reshook)) {
	$moreforfilter .= $hookmanager->resPrint;
} else {
	$moreforfilter = $hookmanager->resPrint;
}

if (!empty($moreforfilter)) {
	print '<div class="liste_titre liste_titre_bydiv centpercent">';
	print $moreforfilter;
	$parameters = array();
	$reshook = $hookmanager->executeHooks('printFieldPreListTitle', $parameters, $object, $action); // Note that $action and $object may have been modified by hook
	print $hookmanager->resPrint;
	print '</div>';
}

$varpage = empty($contextpage) ? $_SERVER["PHP_SELF"] : $contextpage;
$htmlofselectarray = $form->multiSelectArrayWithCheckbox('selectedfields', $arrayfields, $varpage, getDolGlobalString('MAIN_CHECKBOX_LEFT_COLUMN'));  // This also change content of $arrayfields with user setup
$selectedfields = ($mode != 'kanban' ? $htmlofselectarray : '');
$selectedfields .= (count($arrayofmassactions) ? $form->showCheckAddButtons('checkforselect', 1) : '');

print '<div class="div-table-responsive">'; // You can use div-table-responsive-no-min if you don't need reserved height for your table
print '<table class="tagtable nobottomiftotal liste'.($moreforfilter ? " listwithfilterbefore" : "").'">'."\n";

// Fields title search
// --------------------------------------------------------------------
print '<tr class="liste_titre_filter">';
// Action column
if (getDolGlobalString('MAIN_CHECKBOX_LEFT_COLUMN')) {
	print '<td class="liste_titre center maxwidthsearch">';
	$searchpicto = $form->showFilterButtons('left');
	print $searchpicto;
	print '</td>';
}
print '<td class="liste_titre"><input type="text" class="flat" name="search_line" value="'.dol_escape_htmltag($search_line).'" size="6"></td>';
print '<td class="liste_titre"><input type="text" class="flat" name="search_bon" value="'.dol_escape_htmltag($search_bon).'" size="6"></td>';
print '<td class="liste_titre">&nbsp;</td>';
print '<td class="liste_titre"><input type="text" class="flat" name="search_company" value="'.dol_escape_htmltag($search_company).'" size="6"></td>';
print '<td class="liste_titre center"><input type="text" class="flat" name="search_code" value="'.dol_escape_htmltag($search_code).'" size="6"></td>';
print '<td class="liste_titre">&nbsp;</td>';
print '<td class="liste_titre">&nbsp;</td>';
// Action column
if (!getDolGlobalString('MAIN_CHECKBOX_LEFT_COLUMN')) {
	print '<td class="liste_titre center maxwidthsearch">';
	$searchpicto = $form->showFilterButtons();
	print $searchpicto;
	print '</td>';
}
print '</tr>'."\n";

$totalarray = array();
$totalarray['nbfield'] = 0;

$columntitle = "WithdrawalsReceipts";
$columntitlethirdparty = "CustomerCode";
$columncodethirdparty = "s.code_client";
if ($type == 'bank-transfer') {
	$columntitle = "BankTransferReceipts";
	$columntitlethirdparty = "SupplierCode";
	$columncodethirdparty = "s.code_fournisseur";
}

// Fields title label
// --------------------------------------------------------------------
print '<tr class="liste_titre">';
// Action column
if (getDolGlobalString('MAIN_CHECKBOX_LEFT_COLUMN')) {
	print getTitleFieldOfList($selectedfields, 0, $_SERVER["PHP_SELF"], '', '', '', '', $sortfield, $sortorder, 'center maxwidthsearch ')."\n";
	$totalarray['nbfield']++;
}
print_liste_field_titre($columntitle, $_SERVER["PHP_SELF"], "p.ref", '', $param, '', $sortfield, $sortorder);
$totalarray['nbfield']++;
print_liste_field_titre("Line", $_SERVER["PHP_SELF"], '', '', $param, '', $sortfield, $sortorder);
$totalarray['nbfield']++;
print_liste_field_titre(($type == 'bank-transfer' ? "BillsAndSalaries" : "Bills"), $_SERVER["PHP_SELF"], "f.ref", '', $param, '', $sortfield, $sortorder);
$totalarray['nbfield']++;
print_liste_field_titre("ThirdParty", $_SERVER["PHP_SELF"], "s.nom", '', $param, '', $sortfield, $sortorder);
$totalarray['nbfield']++;
print_liste_field_titre($columntitlethirdparty, $_SERVER["PHP_SELF"], $columncodethirdparty, '', $param, '', $sortfield, $sortorder, 'center ');
$totalarray['nbfield']++;
print_liste_field_titre("Date", $_SERVER["PHP_SELF"], "p.datec", "", $param, '', $sortfield, $sortorder, 'center ');
$totalarray['nbfield']++;
print_liste_field_titre("Amount", $_SERVER["PHP_SELF"], "pl.amount", "", $param, '', $sortfield, $sortorder, 'right ');
$totalarray['nbfield']++;
// Action column
if (!getDolGlobalString('MAIN_CHECKBOX_LEFT_COLUMN')) {
	print getTitleFieldOfList($selectedfields, 0, $_SERVER["PHP_SELF"], '', '', '', '', $sortfield, $sortorder, 'center maxwidthsearch ')."\n";
	$totalarray['nbfield']++;
}
print '</tr>'."\n";

// Loop on record
// --------------------------------------------------------------------
$i = 0;
$savnbfield = $totalarray['nbfield'];
$totalarray = array();
$totalarray['nbfield'] = 0;

$imaxinloop = ($limit ? min($num, $limit) : $num);
while ($i < $imaxinloop) {
	$obj = $db->fetch_object($resql);

	$bon->id = $obj->rowid;
	$bon->ref = $obj->ref;
	$bon->statut = $obj->status;
	$bon->date_echeance = $obj->datec;
	$bon->total = $obj->amount;

	$object = $bon;
	if ($object->checkIfSalaryBonPrelevement()) {
		$fullname = explode(' ', $obj->name);

		$userstatic->id = $obj->socid;
		$userstatic->email = $obj->email;
		$userstatic->firstname = $fullname[0];
		$userstatic->lastname = isset($fullname[1]) ? $fullname[1] : '';
	}

	$company->id = $obj->socid;
	$company->name = $obj->name;
	$company->email = $obj->email;
	$company->code_client = $obj->code_client;

	if ($mode == 'kanban') {
		if ($i == 0) {
			print '<tr class="trkanban"><td colspan="'.$savnbfield.'">';
			print '<div class="box-flex-container kanban">';
		}
		// Output Kanban
		$selected = -1;
		if ($massactionbutton || $massaction) { // If we are in select mode (massactionbutton defined) or if we have already selected and sent an action ($massaction) defined
			$selected = 0;
			if (in_array($object->id, $arrayofselected)) {
				$selected = 1;
			}
		}
		print $object->getKanbanView('', array('selected' => $selected));
		if ($i == ($imaxinloop - 1)) {
			print '</div>';
			print '</td></tr>';
		}
	} else {
		// Show line of result
		$j = 0;
		print '<tr data-rowid="'.$object->id.'" class="oddeven">';

		// Action column
		if (getDolGlobalString('MAIN_CHECKBOX_LEFT_COLUMN')) {
			print '<td class="nowrap center">';
			if ($massactionbutton || $massaction) { // If we are in select mode (massactionbutton defined) or if we have already selected and sent an action ($massaction) defined
				$selected = 0;
				if (in_array($object->id, $arrayofselected)) {
					$selected = 1;
				}
				print '<input id="cb'.$object->id.'" class="flat checkforselect" type="checkbox" name="toselect[]" value="'.$object->id.'"'.($selected ? ' checked="checked"' : '').'>';
			}
			print '</td>';
			if (!$i) {
				$totalarray['nbfield']++;
			}
		}
		print '<td>';
		print $bon->getNomUrl(1);
		print "</td>\n";

		print '<td>';
		print $line->LibStatut($obj->statut_ligne, 2);
		print "&nbsp;";
		print '<a href="'.DOL_URL_ROOT.'/compta/prelevement/line.php?id='.$obj->rowid_ligne.'">';
		print substr('000000'.$obj->rowid_ligne, -6);
		print '</a></td>';

		// Ref invoice or salary
		print '<td class="nowraponall">';
		$link_to_bill = '/compta/facture/card.php?facid=';
		$link_title = 'Invoice';
		$link_picto = 'bill';
		if ($type == 'bank-transfer') {
			if ($bon->checkIfSalaryBonPrelevement()) {
				$link_to_bill = '/salaries/card.php?id=';
				$link_title = 'SalaryInvoice';
				$link_picto = 'salary';
			} else {
				$link_to_bill = '/fourn/facture/card.php?facid=';
				$link_title = 'SupplierInvoice';
				$link_picto = 'supplier_invoice';
			}
		}
		print '<a href="'.DOL_URL_ROOT.$link_to_bill.$obj->facid.'">';
		print img_object($langs->trans($link_title), $link_picto);
		if (!$bon->checkIfSalaryBonPrelevement()) {
			print '&nbsp;'.$obj->invoiceref."</td>\n";
		} else {
			print '&nbsp;'.(!empty($obj->invoiceref) ? $obj->invoiceref : $obj->facid)."</td>\n";
		}
		print '</a>';
		print '</td>';

		// Thirdparty (company or user)
		print '<td class="tdoverflowmax150">';
		print(!$bon->checkIfSalaryBonPrelevement() ? $company->getNomUrl(1) : $userstatic->getNomUrl(-1));
		print "</td>\n";

		print '<td class="center">';
		$link_to_tab = '/comm/card.php?socid=';
		$link_code = $obj->code_client;
		if ($type == 'bank-transfer') {
			$link_to_tab = '/fourn/card.php?socid=';
			$link_code = $obj->code_fournisseur;
		}
		print '<a href="'.DOL_URL_ROOT.$link_to_tab.$company->id.'">'.$link_code."</a>";
		print "</td>\n";

		print '<td class="center">'.dol_print_date($db->jdate($obj->datec), 'day')."</td>\n";

		print '<td class="nowraponall right"><span class="amount">'.price($obj->amount)."</span></td>\n";

		// Action column
		if (!getDolGlobalString('MAIN_CHECKBOX_LEFT_COLUMN')) {
			print '<td class="nowrap center">';
			if ($massactionbutton || $massaction) { // If we are in select mode (massactionbutton defined) or if we have already selected and sent an action ($massaction) defined
				$selected = 0;
				if (in_array($object->id, $arrayofselected)) {
					$selected = 1;
				}
				print '<input id="cb'.$object->id.'" class="flat checkforselect" type="checkbox" name="toselect[]" value="'.$object->id.'"'.($selected ? ' checked="checked"' : '').'>';
			}
			print '</td>';
			if (!$i) {
				$totalarray['nbfield']++;
			}
		}

		print '</tr>'."\n";
	}
	$i++;
}

if ($num == 0) {
	print '<tr><td colspan="8"><span class="opacitymedium">'.$langs->trans("None").'</span></td></tr>';
}

$db->free($resql);

$parameters = array('arrayfields' => $arrayfields, 'sql' => $sql);
$reshook = $hookmanager->executeHooks('printFieldListFooter', $parameters, $object, $action); // Note that $action and $object may have been modified by hook
print $hookmanager->resPrint;

print '</table>'."\n";
print '</div>'."\n";

print '</form>'."\n";


// End of page
llxFooter();
$db->close();
